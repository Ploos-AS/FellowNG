#pragma once

#include "Driver/ISoundDriver.h"
#include "Service/IFileops.h"
#include "Service/IHud.h"
#include "Service/ILog.h"
#include "Service/IRetroPlatform.h"

class ICorePlatformFactory
{
public:
  virtual ~ICorePlatformFactory() = default;

  virtual ISoundDriver *CreateSoundDriver() = 0;
  virtual Service::IFileops *CreateFileops(Service::ILog *log) = 0;
  virtual Service::IHud *CreateHud() = 0;
  virtual Service::IRetroPlatform *CreateRetroPlatform() = 0;
};
