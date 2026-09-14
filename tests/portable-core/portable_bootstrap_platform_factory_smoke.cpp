#include <cassert>

#include "VirtualHost/PortableBootstrapPlatformFactory.h"

int main()
{
  auto *factory = FellowNG::VirtualHost::GetPortableBootstrapPlatformFactory();
  assert(factory != nullptr);

  auto *sound = factory->CreateSoundDriver();
  auto *fileops = factory->CreateFileops(nullptr);
  auto *hud = factory->CreateHud();
  auto *retro = factory->CreateRetroPlatform();

  assert(sound != nullptr);
  assert(fileops != nullptr);
  assert(hud != nullptr);
  assert(retro != nullptr);

  SoundDriverRuntimeConfiguration configuration{};
  assert(sound->EmulationStart(configuration));
  assert(sound->IsInitialized());
  sound->EmulationStop();
  assert(!sound->IsInitialized());

  char path[::Service::FILEOPS_MAX_FILE_PATH]{};
  assert(fileops->GetDefaultConfigFileName(path));
  hud->SetHarddiskLED(0, true, false);
  assert(retro->SendHardDriveContent(0, "disk.hdf", false));

  delete sound;
  delete fileops;
  delete hud;
  delete retro;
  return 0;
}
