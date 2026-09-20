#include "VirtualHost/Core.h"
#include "VirtualHost/CoreFactory.h"

#include "Service/Log.h"
#include "Service/FileInformation.h"

#include "hardfile/HardfileHandler.h"

#include "DebugApi/M68K.h"
#include "DebugApi/MemorySystem.h"

#if defined(_WIN32) && !defined(FELLOWNG_PORTABLE_FRONTEND)
#include "VirtualHost/WindowsCorePlatformFactory.h"
#else
#include "VirtualHost/PortableBootstrapPlatformFactory.h"
#endif

using namespace Service;
using namespace Debug;
using namespace fellow::hardfile;

namespace
{
  ICorePlatformFactory *DefaultPlatformFactory()
  {
#if defined(_WIN32) && !defined(FELLOWNG_PORTABLE_FRONTEND)
    return GetWindowsCorePlatformFactory();
#else
    return FellowNG::VirtualHost::GetPortableBootstrapPlatformFactory();
#endif
  }

  ICorePlatformFactory *platform_factory = DefaultPlatformFactory();
}

void CoreFactory::SetPlatformFactory(ICorePlatformFactory *factory)
{
  platform_factory = factory == nullptr ? DefaultPlatformFactory() : factory;
}

ICorePlatformFactory *CoreFactory::GetPlatformFactory()
{
  return platform_factory;
}

void CoreFactory::CreateDrivers()
{
  _core.Drivers.SoundDriver = platform_factory->CreateSoundDriver();
}

void CoreFactory::DestroyDrivers()
{
  delete _core.Drivers.SoundDriver;
  _core.Drivers.SoundDriver = nullptr;
}

void CoreFactory::CreateServices()
{
  _core.Log = new Log();
  _core.Fileops = platform_factory->CreateFileops(_core.Log);
  _core.FileInformation = new FileInformation();
  _core.Hud = platform_factory->CreateHud();
  _core.RP = platform_factory->CreateRetroPlatform();
}

void CoreFactory::DestroyServices()
{
  delete _core.RP;
  _core.RP = nullptr;

  delete _core.Hud;
  _core.Hud = nullptr;

  delete _core.Fileops;
  _core.Fileops = nullptr;

  delete _core.Log;
  _core.Log = nullptr;

  delete _core.FileInformation;
  _core.FileInformation = nullptr;
}

void CoreFactory::CreateModules()
{
  _core.Sound = new Sound();
  _core.Uart = new Uart();
  _core.RtcOkiMsm6242rs = new RtcOkiMsm6242rs(_core.Log);
  _core.HardfileHandler = new HardfileHandler(*_core.DebugVM.Memory, *_core.DebugVM.CPU, *_core.Log);
}

void CoreFactory::DestroyModules()
{
  delete _core.HardfileHandler;
  _core.HardfileHandler = nullptr;

  delete _core.Uart;
  _core.Uart = nullptr;

  delete _core.Sound;
  _core.Sound = nullptr;

  delete _core.RtcOkiMsm6242rs;
  _core.RtcOkiMsm6242rs = nullptr;
}

void CoreFactory::CreateDebugVM()
{
  _core.DebugVM.CPU = new M68K();
  _core.DebugVM.Memory = new MemorySystem();
}

void CoreFactory::DestroyDebugVM()
{
  delete _core.DebugVM.Memory;
  _core.DebugVM.Memory = nullptr;

  delete _core.DebugVM.CPU;
  _core.DebugVM.CPU = nullptr;
}
