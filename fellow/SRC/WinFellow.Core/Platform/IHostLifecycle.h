#pragma once

namespace FellowNG::Platform
{
  class IHostLifecycle
  {
  public:
    virtual ~IHostLifecycle() = default;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
  };
}
