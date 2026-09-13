#pragma once

#include <chrono>

namespace FellowNG::Platform
{
  class IClock
  {
  public:
    using duration = std::chrono::steady_clock::duration;
    using time_point = std::chrono::steady_clock::time_point;

    virtual ~IClock() = default;
    virtual time_point Now() const noexcept = 0;
    virtual void SleepFor(duration duration) const = 0;
  };
}
