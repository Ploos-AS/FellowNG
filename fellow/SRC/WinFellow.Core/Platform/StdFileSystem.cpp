#include "Platform/StdFileSystem.h"

#include <system_error>

namespace FellowNG::Platform
{
  FileStat StdFileSystem::Stat(const std::filesystem::path &path) const noexcept
  {
    FileStat result{};
    std::error_code ec;
    const auto status = std::filesystem::status(path, ec);
    if (ec)
    {
      return result;
    }

    result.exists = std::filesystem::exists(status);
    if (!result.exists)
    {
      return result;
    }

    result.regular_file = std::filesystem::is_regular_file(status);
    result.directory = std::filesystem::is_directory(status);

    const auto permissions = status.permissions();
    result.readable =
      (permissions & (std::filesystem::perms::owner_read |
                      std::filesystem::perms::group_read |
                      std::filesystem::perms::others_read)) != std::filesystem::perms::none;
    result.writable =
      (permissions & (std::filesystem::perms::owner_write |
                      std::filesystem::perms::group_write |
                      std::filesystem::perms::others_write)) != std::filesystem::perms::none;

    if (result.regular_file)
    {
      result.size = std::filesystem::file_size(path, ec);
      if (ec)
      {
        result.size = 0;
      }
    }

    return result;
  }

  std::filesystem::path StdFileSystem::Absolute(const std::filesystem::path &path) const
  {
    return std::filesystem::absolute(path);
  }
}
