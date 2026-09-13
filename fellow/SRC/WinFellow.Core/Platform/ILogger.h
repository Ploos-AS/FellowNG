#pragma once

#include <string_view>

namespace FellowNG::Platform
{
  enum class LogLevel
  {
    Debug,
    Information,
    Warning,
    Error
  };

  class ILogger
  {
  public:
    virtual ~ILogger() = default;
    virtual void Write(LogLevel level, std::string_view message) = 0;
  };
}
