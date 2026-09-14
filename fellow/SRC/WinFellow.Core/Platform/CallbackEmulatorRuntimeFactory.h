#pragma once

#include <memory>

#include "IEmulatorRuntimeFactory.h"

namespace FellowNG::Platform
{
  class CallbackEmulatorRuntimeFactory final : public IEmulatorRuntimeFactory
  {
  public:
    using CreateCallback = std::unique_ptr<IEmulatorRuntime> (*)();

    explicit CallbackEmulatorRuntimeFactory(CreateCallback create_callback)
      : _create_callback(create_callback)
    {
    }

    std::unique_ptr<IEmulatorRuntime> Create() override
    {
      if (_create_callback == nullptr) return nullptr;
      return _create_callback();
    }

  private:
    CreateCallback _create_callback;
  };
}
