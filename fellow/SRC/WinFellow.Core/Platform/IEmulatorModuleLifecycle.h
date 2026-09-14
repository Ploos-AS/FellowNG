#pragma once

namespace FellowNG::Platform
{
  class IEmulatorModuleLifecycle
  {
  public:
    virtual ~IEmulatorModuleLifecycle() = default;

    virtual bool Start(int argc, const char **argv) = 0;
    virtual void Stop() = 0;
    virtual bool IsStarted() const = 0;
  };
}
