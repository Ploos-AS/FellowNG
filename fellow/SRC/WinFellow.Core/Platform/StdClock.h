#pragma once

#include "Platform/IClock.h"

namespace FellowNG::Platform
{
  class StdClock final : public IClock
  {
  public:
    time_point Now() const noexcept override;
    void SleepFor(duration duration) const override;
  };
}
