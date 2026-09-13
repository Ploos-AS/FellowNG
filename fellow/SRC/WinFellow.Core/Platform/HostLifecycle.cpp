#include "Platform/HostLifecycle.h"

namespace FellowNG::Platform
{
  HostLifecycle::HostLifecycle(ILogger &logger) : _logger(logger)
  {
  }

  bool HostLifecycle::Start()
  {
    if (_running)
    {
      _logger.Write(LogLevel::Debug, "host already running");
      return true;
    }

    _logger.Write(LogLevel::Information, "host starting");
    _running = true;
    _logger.Write(LogLevel::Information, "host started");
    return true;
  }

  void HostLifecycle::Stop()
  {
    if (!_running)
    {
      _logger.Write(LogLevel::Debug, "host already stopped");
      return;
    }

    _logger.Write(LogLevel::Information, "host stopping");
    _running = false;
    _logger.Write(LogLevel::Information, "host stopped");
  }

  bool HostLifecycle::IsRunning() const
  {
    return _running;
  }
}
