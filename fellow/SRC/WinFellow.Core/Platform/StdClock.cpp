#include "Platform/StdClock.h"

#include <thread>

namespace FellowNG::Platform
{
  IClock::time_point StdClock::Now() const noexcept
  {
    return std::chrono::steady_clock::now();
  }

  void StdClock::SleepFor(const duration duration) const
  {
    if (duration > duration::zero())
    {
      std::this_thread::sleep_for(duration);
    }
  }
}
