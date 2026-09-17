#pragma once

// Minimal non-Windows compatibility for the small set of MSVC secure CRT
// helpers still used by legacy WinFellow sources. Keep this out of Windows
// builds so the native CRT remains authoritative there.
#if !defined(_WIN32)
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static inline char *fellow_strdup(const char *source)
{
  if (source == nullptr)
  {
    return nullptr;
  }

  const std::size_t length = std::strlen(source) + 1;
  char *copy = static_cast<char *>(std::malloc(length));
  if (copy != nullptr)
  {
    std::memcpy(copy, source, length);
  }
  return copy;
}

#ifndef _strdup
#define _strdup fellow_strdup
#endif

static inline int fopen_s(FILE **stream, const char *filename, const char *mode)
{
  if (stream == nullptr)
  {
    return EINVAL;
  }

  *stream = std::fopen(filename, mode);
  return (*stream != nullptr) ? 0 : errno;
}

template <std::size_t N>
static inline int strcpy_s(char (&destination)[N], const char *source)
{
  if (source == nullptr)
  {
    if (N != 0)
    {
      destination[0] = '\0';
    }
    return EINVAL;
  }

  const std::size_t length = std::strlen(source);
  if (length >= N)
  {
    if (N != 0)
    {
      destination[0] = '\0';
    }
    return ERANGE;
  }

  std::memcpy(destination, source, length + 1);
  return 0;
}
#endif
