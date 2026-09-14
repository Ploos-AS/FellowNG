#pragma once

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>

#include "Service/IFileops.h"
#include "Service/ILog.h"

namespace FellowNG::Service
{
  class PortableFileops final : public ::Service::IFileops
  {
  public:
    explicit PortableFileops(::Service::ILog *) {}

    bool GetFellowLogfileName(char *target) override { return Copy(target, "fellow.log"); }
    bool GetGenericFileName(char *target, const char *name, const char *extension) override
    {
      if (target == nullptr || name == nullptr) return false;
      std::string value{name};
      if (extension != nullptr && *extension != '\0')
      {
        if (*extension != '.') value += '.';
        value += extension;
      }
      return Copy(target, value.c_str());
    }
    bool GetDefaultConfigFileName(char *target) override { return Copy(target, "fellow.ini"); }
    bool ResolveVariables(const char *source, char *target) override
    {
      if (source == nullptr) return false;
      return Copy(target, source);
    }
    bool GetWinFellowPresetPath(char *target, const uint32_t size) override { return CopySized(target, size, "."); }
    bool GetScreenshotFileName(char *target) override { return Copy(target, "screenshot.bmp"); }
    char *GetTemporaryFilename() override
    {
      static thread_local char path[::Service::FILEOPS_MAX_FILE_PATH]{};
      const auto value = std::filesystem::temp_directory_path() / "fellowng.tmp";
      Copy(path, value.string().c_str());
      return path;
    }
    bool GetWinFellowInstallationPath(char *target, const uint32_t size) override
    {
      return CopySized(target, size, std::filesystem::current_path().string().c_str());
    }
    bool GetKickstartByCRC32(const char *, const uint32_t, char *, const uint32_t) override { return false; }

  private:
    static bool Copy(char *target, const char *source)
    {
      return CopySized(target, ::Service::FILEOPS_MAX_FILE_PATH, source);
    }

    static bool CopySized(char *target, const uint32_t size, const char *source)
    {
      if (target == nullptr || source == nullptr || size == 0) return false;
      const auto length = std::strlen(source);
      if (length + 1 > size) return false;
      std::memcpy(target, source, length + 1);
      return true;
    }
  };
}
