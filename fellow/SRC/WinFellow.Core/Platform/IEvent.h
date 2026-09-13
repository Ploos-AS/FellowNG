#pragma once

#include <chrono>

namespace FellowNG::Platform
{
  class IEvent
  {
  public:
    virtual ~IEvent() = default;
    virtual void Set() = 0;
    virtual void Reset() = 0;
    virtual void Wait() = 0;
    virtual bool WaitFor(std::chrono::milliseconds timeout) = 0;
  };
}
