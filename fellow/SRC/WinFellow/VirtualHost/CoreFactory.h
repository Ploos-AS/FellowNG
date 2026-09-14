#pragma once

#include "VirtualHost/ICorePlatformFactory.h"

class CoreFactory
{
public:
  static void SetPlatformFactory(ICorePlatformFactory *factory);
  static ICorePlatformFactory *GetPlatformFactory();

  static void CreateDrivers();
  static void DestroyDrivers();
  static void CreateServices();
  static void DestroyServices();
  static void CreateModules();
  static void DestroyModules();
  static void CreateDebugVM();
  static void DestroyDebugVM();
};
