#include "Platform/StreamLogger.h"

namespace FellowNG::Platform
{
  StreamLogger::StreamLogger(std::ostream &stream) : _stream(stream)
  {
  }

  const char *StreamLogger::LevelName(LogLevel level)
  {
    switch (level)
    {
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Information:
      return "INFO";
    case LogLevel::Warning:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    }

    return "UNKNOWN";
  }

  void StreamLogger::Write(LogLevel level, std::string_view message)
  {
    const std::scoped_lock lock(_mutex);
    _stream << '[' << LevelName(level) << "] " << message << '\n';
    _stream.flush();
  }
}
