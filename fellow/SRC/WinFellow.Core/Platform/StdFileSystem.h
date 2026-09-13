#pragma once

#include "Platform/IFileSystem.h"

namespace FellowNG::Platform
{
  class StdFileSystem final : public IFileSystem
  {
  public:
    FileStat Stat(const std::filesystem::path &path) const noexcept override;
    std::filesystem::path Absolute(const std::filesystem::path &path) const override;
  };
}
