#include <cassert>
#include <cstdint>
#include <span>

#include "Platform/IAudioOutput.h"
#include "VirtualHost/PortableCorePlatformFactory.h"

namespace
{
  class Audio final : public FellowNG::Platform::IAudioOutput
  {
  public:
    bool Start(const FellowNG::Platform::AudioFormat &) override { return true; }
    void Stop() override {}
    bool IsRunning() const override { return true; }
    bool SubmitInterleaved(std::span<const std::int16_t>) override { return true; }
  };
}

int main()
{
  Audio audio;
  FellowNG::VirtualHost::PortableCorePlatformFactory factory(audio);

  auto *sound = factory.CreateSoundDriver();
  auto *fileops = factory.CreateFileops(nullptr);
  auto *hud = factory.CreateHud();
  auto *retro = factory.CreateRetroPlatform();

  assert(sound != nullptr);
  assert(fileops != nullptr);
  assert(hud != nullptr);
  assert(retro != nullptr);

  char path[::Service::FILEOPS_MAX_FILE_PATH]{};
  assert(fileops->GetDefaultConfigFileName(path));
  hud->SetFloppyLED(0, true, false);
  assert(retro->SendHardDriveContent(0, "disk.hdf", true));

  delete sound;
  delete fileops;
  delete hud;
  delete retro;
  return 0;
}
