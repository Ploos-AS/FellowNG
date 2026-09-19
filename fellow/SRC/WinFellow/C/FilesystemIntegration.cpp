/*=========================================================================*/
/* Fellow                                                                  */
/* Filesystem wrapper                                                      */
/*                                                                         */
/* Authors: Petter Schau                                                   */
/*          Torsten Enderling (carfesh@gmx.net)                            */
/*          (Wraps code that originates in the UAE project.)               */
/*                                                                         */
/* Copyright (C) 1991, 1992, 1996 Free Software Foundation, Inc.           */
/*                                                                         */
/* This program is free software; you can redistribute it and/or modify    */
/* it under the terms of the GNU General Public License as published by    */
/* the Free Software Foundation; either version 2, or (at your option)     */
/* any later version.                                                      */
/*                                                                         */
/* This program is distributed in the hope that it will be useful,         */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/* GNU General Public License for more details.                            */
/*                                                                         */
/* You should have received a copy of the GNU General Public License       */
/* along with this program; if not, write to the Free Software Foundation, */
/* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.          */
/*=========================================================================*/

#include "Defs.h"
#include "FellowMain.h"
#include "FilesystemIntegration.h"
#include "filesys.h"
#include "VirtualHost/Core.h"

#include <string>

using namespace std;

ffilesys_dev ffilesys_devs[FFILESYS_MAX_DEVICES];
bool ffilesys_enabled;
BOOLE ffilesys_automount_drives;
string ffilesys_device_name_prefix;

BOOLE ffilesysRemoveFilesys(uint32_t index)
{
  BOOLE result = FALSE;
  if (index >= FFILESYS_MAX_DEVICES) return result;
  result = (ffilesys_devs[index].status == ffilesys_status::FFILESYS_INSERTED);
  memset(&(ffilesys_devs[index]), 0, sizeof(ffilesys_dev));
  ffilesys_devs[index].status = ffilesys_status::FFILESYS_NONE;
  return result;
}

void ffilesysSetEnabled(bool enabled) { ffilesys_enabled = enabled; }
bool ffilesysGetEnabled() { return ffilesys_enabled; }
void ffilesysSetFilesys(ffilesys_dev filesys, uint32_t index)
{
  if (index >= FFILESYS_MAX_DEVICES) return;
  ffilesysRemoveFilesys(index);
  ffilesys_devs[index] = filesys;
}
BOOLE ffilesysCompareFilesys(ffilesys_dev filesys, uint32_t index)
{
  if (index >= FFILESYS_MAX_DEVICES) return FALSE;
  size_t len = strlen(filesys.rootpath) - 1;
  if (filesys.rootpath[len] == '\\') filesys.rootpath[len] = '\0';
  return (ffilesys_devs[index].readonly == filesys.readonly) &&
         (strncmp(ffilesys_devs[index].volumename, filesys.volumename, FFILESYS_MAX_VOLUMENAME) == 0) &&
         (strncmp(ffilesys_devs[index].rootpath, filesys.rootpath, CFG_FILENAME_LENGTH) == 0);
}
void ffilesysSetAutomountDrives(BOOLE automount_drives) { ffilesys_automount_drives = automount_drives; }
BOOLE ffilesysGetAutomountDrives() { return ffilesys_automount_drives; }
void ffilesysSetDeviceNamePrefix(const string &prefix) { ffilesys_device_name_prefix = prefix; }
const string &ffilesysGetDeviceNamePrefix() { return ffilesys_device_name_prefix; }

static BOOLE ffilesysHasZeroDevices()
{
  uint32_t dev_count = 0;
  for (uint32_t i = 0; i < FFILESYS_MAX_DEVICES; i++)
    if (ffilesys_devs[i].status == ffilesys_status::FFILESYS_INSERTED) dev_count++;
  return (dev_count == 0) && !ffilesysGetAutomountDrives();
}

void ffilesysClear()
{
  for (uint32_t i = 0; i < FFILESYS_MAX_DEVICES; i++) ffilesysRemoveFilesys(i);
}

void ffilesysDumpConfig()
{
  for (uint32_t i = 0; i < FFILESYS_MAX_DEVICES; i++)
  {
    if (ffilesys_devs[i].status == ffilesys_status::FFILESYS_INSERTED)
    {
      fprintf(stderr, "filesystem: Slot: %u, %s, %s, %s\\n",
              i, ffilesys_devs[i].volumename, ffilesys_devs[i].rootpath,
              (ffilesys_devs[i].readonly) ? "R" : "RW");
    }
  }
  fflush(stderr);
}

void ffilesysInstall()
{
  fprintf(stderr, "filesystem: install begin mount_units=%d\\n", mountinfo.num_units);
  for (uint32_t i = 0; i < FFILESYS_MAX_DEVICES; i++)
    if (ffilesys_devs[i].status == ffilesys_status::FFILESYS_INSERTED)
    {
      size_t len = strlen(ffilesys_devs[i].rootpath) - 1;
      if (ffilesys_devs[i].rootpath[len] == '\\') ffilesys_devs[i].rootpath[len] = '\0';
      add_filesys_unit(&mountinfo, ffilesys_devs[i].volumename, ffilesys_devs[i].rootpath, ffilesys_devs[i].readonly, 0, 0, 0, 0);
      fprintf(stderr, "filesystem: installed slot=%u volume=%s mount_units=%d\\n",
              i, ffilesys_devs[i].volumename, mountinfo.num_units);
    }
  fprintf(stderr, "filesystem: install end mount_units=%d\\n", mountinfo.num_units);
  fflush(stderr);
}

void ffilesysHardReset()
{
  const BOOLE zero_devices = ffilesysHasZeroDevices();
  const BOOLE enabled = ffilesysGetEnabled();
  const uint32_t kick_version = memoryGetKickImageVersion();
  fprintf(stderr, "filesystem: hard-reset zero_devices=%d enabled=%d kick_version=%u mount_units=%d\\n",
          zero_devices, enabled, kick_version, mountinfo.num_units);
  if ((!zero_devices) && enabled && (kick_version > 36))
  {
    rtarea_setup();
    rtarea_init();
    hardfile_install();
    filesys_install();
    /* The imported UAE filesystem core no longer exposes filesys_init().
       Explicitly configured units are installed below through ffilesysInstall(). */
    filesys_prepare_reset();
    filesys_reset();

    // A hard reset rebuilds the UAE filesystem configuration. Do not append
    // the configured Fellow units to mountinfo repeatedly across resets.
    ffilesysClearMountinfo();
    ffilesysInstall();
    filesys_start_threads();
    memoryEmemCardAdd(expamem_init_filesys, expamem_map_filesys);
  }
}

void ffilesysEmulationStart()
{
#ifdef _DEBUG
  ffilesysDumpConfig();
#endif
}
void ffilesysEmulationStop() {}
void ffilesysStartup()
{
  ffilesysClear();
  ffilesysSetAutomountDrives(FALSE);
  ffilesysSetDeviceNamePrefix("FS");
}
void ffilesysClearMountinfo()
{
  for (mountinfo.num_units; mountinfo.num_units > 0; mountinfo.num_units--)
  {
    if (mountinfo.ui[mountinfo.num_units - 1].volname)
    {
      free(mountinfo.ui[mountinfo.num_units - 1].volname);
      mountinfo.ui[mountinfo.num_units - 1].volname = nullptr;
    }
    if (mountinfo.ui[mountinfo.num_units - 1].rootdir)
    {
      free(mountinfo.ui[mountinfo.num_units - 1].rootdir);
      mountinfo.ui[mountinfo.num_units - 1].rootdir = nullptr;
    }
  }
}
void ffilesysShutdown()
{
  filesys_prepare_reset();
  filesys_reset();
  ffilesysClearMountinfo();
}
