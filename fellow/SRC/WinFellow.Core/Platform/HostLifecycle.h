#pragma once

#include "Platform/IHostLifecycle.h"
#include "Platform/ILogger.h"

namespace FellowNG::Platform
{
  class HostLifecycle final : public IHostLifecycle
  {
  public:
    explicit HostLifecycle(ILogger &logger);

    bool Start() override;
    void Stop() override;
    bool IsRunning() const override;

  private:
    ILogger &_logger;
    bool _running{false};
  };
}
