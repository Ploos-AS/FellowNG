#pragma once

#include <memory>

#include "IEmulatorRuntime.h"

namespace FellowNG::Platform
{
  class IEmulatorRuntimeFactory
  {
  public:
    virtual ~IEmulatorRuntimeFactory() = default;

    virtual std::unique_ptr<IEmulatorRuntime> Create() = 0;
  };
}
