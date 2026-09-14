#pragma once

#include <memory>

#include "FellowMain.h"
#include "Platform/IEmulatorRuntimeFactory.h"

namespace FellowNG::Runtime
{
  class WinFellowRuntimeFactory final : public Platform::IEmulatorRuntimeFactory
  {
  public:
    WinFellowRuntimeFactory(
        int argc = 0,
        const char **argv = nullptr,
        WinFellowRuntime::ModulesStartup modules_startup = nullptr,
        WinFellowRuntime::ModulesShutdown modules_shutdown = nullptr)
      : _argc(argc),
        _argv(argv),
        _modules_startup(modules_startup),
        _modules_shutdown(modules_shutdown)
    {
    }

    std::unique_ptr<Platform::IEmulatorRuntime> Create() override
    {
      return std::make_unique<WinFellowRuntime>(_argc, _argv, _modules_startup, _modules_shutdown);
    }

  private:
    int _argc = 0;
    const char **_argv = nullptr;
    WinFellowRuntime::ModulesStartup _modules_startup = nullptr;
    WinFellowRuntime::ModulesShutdown _modules_shutdown = nullptr;
  };
}
