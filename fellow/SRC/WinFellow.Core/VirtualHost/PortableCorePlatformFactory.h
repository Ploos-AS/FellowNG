#pragma once

#include "Driver/PortableSoundDriver.h"
#include "Platform/IAudioOutput.h"
#include "Service/NullHud.h"
#include "Service/NullRetroPlatform.h"
#include "Service/PortableFileops.h"
#include "VirtualHost/ICorePlatformFactory.h"

namespace FellowNG::VirtualHost
{
  class PortableCorePlatformFactory final : public ::ICorePlatformFactory
  {
  public:
    explicit PortableCorePlatformFactory(Platform::IAudioOutput &audio_output)
      : _audio_output(audio_output)
    {
    }

    ISoundDriver *CreateSoundDriver() override
    {
      return new Driver::PortableSoundDriver(_audio_output);
    }

    ::Service::IFileops *CreateFileops(::Service::ILog *log) override
    {
      return new FellowNG::Service::PortableFileops(log);
    }

    ::Service::IHud *CreateHud() override
    {
      return new FellowNG::Service::NullHud();
    }

    ::Service::IRetroPlatform *CreateRetroPlatform() override
    {
      return new FellowNG::Service::NullRetroPlatform();
    }

  private:
    Platform::IAudioOutput &_audio_output;
  };
}
