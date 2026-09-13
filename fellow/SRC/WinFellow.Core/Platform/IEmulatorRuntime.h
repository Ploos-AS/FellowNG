#pragma once

#include "IAudioOutput.h"
#include "IInputSource.h"
#include "IVideoOutput.h"

namespace FellowNG::Platform
{
  class IEmulatorRuntime
  {
  public:
    virtual ~IEmulatorRuntime() = default;

    virtual bool Start(IVideoOutput &video, IAudioOutput &audio) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual void HandleInput(const InputEvent &event) = 0;
    virtual bool RunSlice() = 0;
  };
}
