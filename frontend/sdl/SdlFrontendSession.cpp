#include "SdlFrontendSession.h"

namespace FellowNG::Frontend::SDL
{
SdlFrontendSession::SdlFrontendSession(Platform::IEmulatorRuntime &runtime,
                                       Platform::IInputSource &input,
                                       Platform::IVideoOutput &video,
                                       Platform::IAudioOutput &audio)
    : _borrowed_session(std::make_unique<Platform::FrontendSession>(runtime, input, video, audio)),
      _session(_borrowed_session.get())
{
}

SdlFrontendSession::SdlFrontendSession(Platform::IEmulatorRuntimeFactory &runtime_factory,
                                       Platform::IInputSource &input,
                                       Platform::IVideoOutput &video,
                                       Platform::IAudioOutput &audio)
    : _owned_runtime(runtime_factory.Create())
{
  if (_owned_runtime)
  {
    _owned_session = std::make_unique<Platform::FrontendSession>(*_owned_runtime, input, video, audio);
    _session = _owned_session.get();
  }
}

bool SdlFrontendSession::Start()
{
  return _session != nullptr && _session->Start();
}

bool SdlFrontendSession::PumpOnce()
{
  return _session != nullptr && _session->PumpOnce();
}

void SdlFrontendSession::Stop()
{
  if (_session != nullptr) _session->Stop();
}

bool SdlFrontendSession::IsRunning() const
{
  return _session != nullptr && _session->IsRunning();
}

bool SdlFrontendSession::HasRuntime() const
{
  return _session != nullptr;
}
}
