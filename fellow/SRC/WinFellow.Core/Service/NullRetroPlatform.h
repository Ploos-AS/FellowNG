#pragma once

#include "Service/IRetroPlatform.h"

namespace FellowNG::Service
{
  class NullRetroPlatform final : public ::Service::IRetroPlatform
  {
  public:
    bool SendHardDriveContent(const uint32_t, const char *, const bool) override
    {
      return true;
    }
  };
}
