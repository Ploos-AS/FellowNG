#pragma once

#include "IEmulatorRuntime.h"

namespace FellowNG::Platform
{
  class FrontendSession final
  {
  public:
    FrontendSession(IEmulatorRuntime &runtime, IInputSource &input, IVideoOutput &video, IAudioOutput &audio);

    bool Start();
    bool PumpOnce();
    void Stop();
    bool IsRunning() const;

  private:
    IEmulatorRuntime &_runtime;
    IInputSource &_input;
    IVideoOutput &_video;
    IAudioOutput &_audio;
  };
}
