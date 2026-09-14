#pragma once

#include <memory>

#include "Platform/FrontendSession.h"
#include "Platform/IEmulatorRuntimeFactory.h"

namespace FellowNG::Frontend::SDL
{
class SdlFrontendSession
{
public:
  SdlFrontendSession(Platform::IEmulatorRuntime &runtime, Platform::IInputSource &input, Platform::IVideoOutput &video, Platform::IAudioOutput &audio);
  SdlFrontendSession(Platform::IEmulatorRuntimeFactory &runtime_factory,
                     Platform::IInputSource &input,
                     Platform::IVideoOutput &video,
                     Platform::IAudioOutput &audio);

  bool Start();
  bool PumpOnce();
  void Stop();
  bool IsRunning() const;
  bool HasRuntime() const;

private:
  std::unique_ptr<Platform::IEmulatorRuntime> _owned_runtime;
  std::unique_ptr<Platform::FrontendSession> _owned_session;
  Platform::FrontendSession *_session = nullptr;
  std::unique_ptr<Platform::FrontendSession> _borrowed_session;
};
}
