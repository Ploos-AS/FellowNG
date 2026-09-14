#pragma once

#include "Platform/IEmulatorModuleLifecycle.h"

namespace FellowNG::Platform
{
  class CallbackEmulatorModuleLifecycle final : public IEmulatorModuleLifecycle
  {
  public:
    using Startup = bool (*)(int argc, const char **argv);
    using Shutdown = void (*)();

    CallbackEmulatorModuleLifecycle(Startup startup, Shutdown shutdown)
      : _startup(startup), _shutdown(shutdown)
    {
    }

    bool Start(int argc, const char **argv) override
    {
      if (_started)
      {
        return true;
      }

      if (_startup == nullptr || !_startup(argc, argv))
      {
        return false;
      }

      _started = true;
      return true;
    }

    void Stop() override
    {
      if (!_started)
      {
        return;
      }

      if (_shutdown != nullptr)
      {
        _shutdown();
      }

      _started = false;
    }

    bool IsStarted() const override
    {
      return _started;
    }

  private:
    Startup _startup = nullptr;
    Shutdown _shutdown = nullptr;
    bool _started = false;
  };
}
