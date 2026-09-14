#pragma once

#include "Driver/PortableSoundDriver.h"
#include "Platform/IAudioOutput.h"
#include "VirtualHost/ICorePlatformFactory.h"

class PortableAudioPlatformFactory final : public ICorePlatformFactory
{
public:
  PortableAudioPlatformFactory(ICorePlatformFactory &fallback, FellowNG::Platform::IAudioOutput &audio_output)
    : _fallback(fallback), _audio_output(audio_output)
  {
  }

  ISoundDriver *CreateSoundDriver() override
  {
    return new FellowNG::Driver::PortableSoundDriver(_audio_output);
  }

  Service::IFileops *CreateFileops(Service::ILog *log) override
  {
    return _fallback.CreateFileops(log);
  }

  Service::IHud *CreateHud() override
  {
    return _fallback.CreateHud();
  }

  Service::IRetroPlatform *CreateRetroPlatform() override
  {
    return _fallback.CreateRetroPlatform();
  }

private:
  ICorePlatformFactory &_fallback;
  FellowNG::Platform::IAudioOutput &_audio_output;
};
