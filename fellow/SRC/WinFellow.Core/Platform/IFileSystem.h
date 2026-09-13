#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

namespace FellowNG::Platform
{
  struct FileStat
  {
    bool exists{false};
    bool regular_file{false};
    bool directory{false};
    bool readable{false};
    bool writable{false};
    std::uintmax_t size{0};
  };

  class IFileSystem
  {
  public:
    virtual ~IFileSystem() = default;
    virtual FileStat Stat(const std::filesystem::path &path) const noexcept = 0;
    virtual std::filesystem::path Absolute(const std::filesystem::path &path) const = 0;
  };
}
