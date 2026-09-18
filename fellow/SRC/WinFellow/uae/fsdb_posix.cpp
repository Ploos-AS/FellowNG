/*
 * Portable POSIX host filesystem support for the legacy UAE fsdb layer.
 * Kept separate from fsdb_win32.c so portable builds do not inherit Win32
 * filename rules or host APIs.
 */
#include "fsdb.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

#include "filesys.h"

uae_u32 filesys_parse_mask(uae_u32 mask)
{
  return mask ^ 0xf;
}

int fsdb_name_invalid(const char *n)
{
  if (n == nullptr || n[0] == '\0') return 1;

  /* '/' is the only character that cannot occur inside a POSIX path
   * component.  Reserve UAE's metadata filename as well. */
  if (std::strchr(n, '/') != nullptr) return 1;
  if (std::strcmp(n, FSDB_FILE) == 0) return 1;
  return 0;
}

void fsdb_fill_file_attrs(a_inode *aino)
{
  struct stat st {};
  if (aino == nullptr || aino->nname == nullptr || stat(aino->nname, &st) != 0) return;

  aino->dir = S_ISDIR(st.st_mode) ? 1U : 0U;

  int amiga_mode = 0;
  if ((st.st_mode & S_IRUSR) == 0) amiga_mode |= A_FIBF_READ;
  if ((st.st_mode & S_IWUSR) == 0) amiga_mode |= A_FIBF_WRITE | A_FIBF_DELETE;
  if ((st.st_mode & S_IXUSR) == 0) amiga_mode |= A_FIBF_EXECUTE;
  aino->amigaos_mode = amiga_mode;
}

int fsdb_set_file_attrs(a_inode *aino, int mask)
{
  struct stat st {};
  if (aino == nullptr || aino->nname == nullptr || stat(aino->nname, &st) != 0)
    return ERROR_OBJECT_NOT_AROUND;

  mode_t mode = st.st_mode;
  if (mask & A_FIBF_READ) mode &= ~(S_IRUSR | S_IRGRP | S_IROTH);
  else mode |= S_IRUSR;

  if (mask & (A_FIBF_WRITE | A_FIBF_DELETE)) mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
  else mode |= S_IWUSR;

  if (mask & A_FIBF_EXECUTE) mode &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
  else mode |= S_IXUSR;

  if (chmod(aino->nname, mode) != 0) return ERROR_WRITE_PROTECTED;

  aino->amigaos_mode = mask;
  aino->dirty = 1;
  return 0;
}

int fsdb_mode_representable_p(const a_inode *aino)
{
  if (aino == nullptr) return 0;

  /* POSIX mode bits represent Amiga read/write/delete/execute protection.
   * Script, pure and archive have no native POSIX equivalent and therefore
   * require the fsdb metadata sidecar. */
  return (aino->amigaos_mode & (A_FIBF_SCRIPT | A_FIBF_PURE | A_FIBF_ARCHIVE)) == 0;
}

char *fsdb_create_unique_nname(a_inode *base, const char *suggestion)
{
  if (base == nullptr || base->nname == nullptr) return nullptr;

  const char *src = suggestion != nullptr ? suggestion : "unnamed";
  char safe[241] {};
  size_t out = 0;
  for (; *src != '\0' && out < sizeof(safe) - 1; ++src)
    safe[out++] = (*src == '/') ? '_' : *src;

  char candidate[256] {};
  for (unsigned int attempt = 0; ; ++attempt)
  {
    if (attempt == 0)
      std::snprintf(candidate, sizeof(candidate), "__uae___%s", safe);
    else
      std::snprintf(candidate, sizeof(candidate), "__uae___%s_%u", safe, attempt);

    char *p = build_nname(base->nname, candidate);
    if (p == nullptr) return nullptr;
    if (access(p, F_OK) != 0 && errno == ENOENT) return p;
    std::free(p);
  }
}
