#include "SdlFrontendSession.h"

namespace FellowNG::Frontend::SDL
{
SdlFrontendSession::SdlFrontendSession(Platform::IEmulatorRuntime &runtime,
                                       Platform::IInputSource &input,
                                       Platform::IVideoOutput &video,
                                       Platform::IAudioOutput &audio)
    : _session(runtime, input, video, audio)
{
}

bool SdlFrontendSession::Start()
{
  return _session.Start();
}

bool SdlFrontendSession::PumpOnce()
{
  return _session.PumpOnce();
}

void SdlFrontendSession::Stop()
{
  _session.Stop();
}

bool SdlFrontendSession::IsRunning() const
{
  return _session.IsRunning();
}
}
