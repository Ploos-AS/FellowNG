#pragma once

#if !defined(_WIN32)
#error "WindowsCorePlatformFactory is only available on Windows"
#endif

#include "VirtualHost/ICorePlatformFactory.h"

#include "Driver/Sound/DirectSoundDriver.h"
#include "Windows/Service/FileopsWin32.h"
#include "Windows/Service/Hud.h"
#include "Windows/Service/RetroPlatformWrapper.h"

class WindowsCorePlatformFactory final : public ICorePlatformFactory
{
public:
  ISoundDriver *CreateSoundDriver() override
  {
    return new DirectSoundDriver();
  }

  Service::IFileops *CreateFileops(Service::ILog *log) override
  {
    return new FileopsWin32(log);
  }

  Service::IHud *CreateHud() override
  {
    return new Hud();
  }

  Service::IRetroPlatform *CreateRetroPlatform() override
  {
    return new RetroPlatformWrapper();
  }
};

inline ICorePlatformFactory *GetWindowsCorePlatformFactory()
{
  static WindowsCorePlatformFactory factory;
  return &factory;
}
