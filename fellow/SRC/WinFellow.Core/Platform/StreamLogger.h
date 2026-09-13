#pragma once

#include "Platform/ILogger.h"

#include <mutex>
#include <ostream>

namespace FellowNG::Platform
{
  class StreamLogger final : public ILogger
  {
  public:
    explicit StreamLogger(std::ostream &stream);
    void Write(LogLevel level, std::string_view message) override;

  private:
    static const char *LevelName(LogLevel level);

    std::ostream &_stream;
    std::mutex _mutex;
  };
}
