/* fsusage.c -- return space usage of mounted filesystems
   Copyright (C) 1991, 1992, 1996 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* FELLOW IN (START)-----------------

  This file has been adapted for use in WinFellow.
  It originates from the UAE 0.8.22 source code distribution.

  Torsten Enderling (carfesh@gmx.net) 2004

   FELLOW IN (END)------------------- */

/* FELLOW OUT START -----------------------
#include "sysconfig.h"

#include <stdlib.h>
#include <sys/types.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
   FELLOW OUT END   -----------------------*/

/* FELLOW IN START -------------------*/
#include "UAE2FELL.H"
/* FELLOW IN END   -------------------*/

#include "FSUSAGE.H"

/* Return the number of TOSIZE-byte blocks used by
   BLOCKS FROMSIZE-byte blocks, rounding away from zero.
   TOSIZE must be positive.  Return -1 if FROMSIZE is not positive.  */

static long adjust_blocks(long blocks, int fromsize, int tosize)
{
  if (tosize <= 0) abort();
  if (fromsize <= 0) return -1;

  if (fromsize == tosize) /* e.g., from 512 to 512 */
    return blocks;
  else if (fromsize > tosize) /* e.g., from 2048 to 512 */
    return blocks * (fromsize / tosize);
  else /* e.g., from 256 to 512 */
    return (blocks + (blocks < 0 ? -1 : 1)) / (tosize / fromsize);
}

#ifdef _WIN32
/* FELLOW OUT START -----------------------
#include "sysdeps.h"
   FELLOW OUT END   -----------------------*/
#include <windows.h>

int get_fs_usage(const char *path, const char *disk, struct fs_usage *fsp)
{
  /* FELLOW REMOVE: char buf1[1024]; */
  char buf2[1024];
  DWORD SectorsPerCluster;
  DWORD BytesPerSector;
  DWORD NumberOfFreeClusters;
  DWORD TotalNumberOfClusters;

  /* FELLOW CHANGE: fname_atow (path, buf1, sizeof buf1);
  GetFullPathName (buf1, sizeof buf2, buf2, NULL); */
  GetFullPathName(path, sizeof buf2, buf2, NULL);

  buf2[3] = 0;

  if (!GetDiskFreeSpace(buf2, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
  {
    /*lasterror = GetLastError ();*/
    return -1;
  }

  /* HACK ALERT! WinNT returns 0 in TotalNumberOfClusters for an audio-CD, which calls the GURU! */
  if ((TotalNumberOfClusters == 0) && (GetDriveType(buf2) == DRIVE_CDROM))
  {
    TotalNumberOfClusters = 327680;
  }

  BytesPerSector *= SectorsPerCluster;
  fsp->fsu_blocks = adjust_blocks(TotalNumberOfClusters, BytesPerSector, 512);
  fsp->fsu_bavail = adjust_blocks(NumberOfFreeClusters, BytesPerSector, 512);

  return 0;
}

#else /* ! _WIN32 */

#include <sys/statvfs.h>

int get_fs_usage(const char *path, const char *disk, struct fs_usage *fsp)
{
  (void)disk;
  struct statvfs fsd;
  if (statvfs(path, &fsd) < 0) return -1;

  const unsigned long block_size = fsd.f_frsize ? fsd.f_frsize : fsd.f_bsize;
  fsp->fsu_blocks = adjust_blocks((long)fsd.f_blocks, (int)block_size, 512);
  fsp->fsu_bfree = adjust_blocks((long)fsd.f_bfree, (int)block_size, 512);
  fsp->fsu_bavail = adjust_blocks((long)fsd.f_bavail, (int)block_size, 512);
  fsp->fsu_files = (long)fsd.f_files;
  fsp->fsu_ffree = (long)fsd.f_ffree;
  return 0;
}

#endif /* ! _WIN32 */
