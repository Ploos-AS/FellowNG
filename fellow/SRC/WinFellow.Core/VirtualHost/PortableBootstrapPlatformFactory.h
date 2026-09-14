#pragma once

#include "Driver/NullSoundDriver.h"
#include "Service/NullHud.h"
#include "Service/NullRetroPlatform.h"
#include "Service/PortableFileops.h"
#include "VirtualHost/ICorePlatformFactory.h"

namespace FellowNG::VirtualHost
{
  class PortableBootstrapPlatformFactory final : public ::ICorePlatformFactory
  {
  public:
    ISoundDriver *CreateSoundDriver() override
    {
      return new Driver::NullSoundDriver();
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
  };

  inline ::ICorePlatformFactory *GetPortableBootstrapPlatformFactory()
  {
    static PortableBootstrapPlatformFactory factory;
    return &factory;
  }
}
