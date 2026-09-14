#pragma once

#include "Platform/FrontendSession.h"

namespace FellowNG::Frontend::SDL
{
class SdlFrontendSession
{
public:
  SdlFrontendSession(Platform::IEmulatorRuntime &runtime, Platform::IInputSource &input, Platform::IVideoOutput &video, Platform::IAudioOutput &audio);
  bool Start();
  bool PumpOnce();
  void Stop();
  bool IsRunning() const;

private:
  Platform::FrontendSession _session;
};
}
