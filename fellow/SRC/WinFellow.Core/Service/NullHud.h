#pragma once

#include "Service/IHud.h"

namespace FellowNG::Service
{
  class NullHud final : public ::Service::IHud
  {
  public:
    void SetFloppyLED(int, bool, bool) override {}
    void SetHarddiskLED(int, bool, bool) override {}
  };
}
