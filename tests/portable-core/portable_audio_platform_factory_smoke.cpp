#include <cstdint>
#include <span>

#include "Platform/IAudioOutput.h"
#include "VirtualHost/PortableAudioPlatformFactory.h"

namespace
{
  class AudioOutput final : public FellowNG::Platform::IAudioOutput
  {
  public:
    bool Start(const FellowNG::Platform::AudioFormat &) override { running = true; return true; }
    void Stop() override { running = false; }
    bool IsRunning() const override { return running; }
    bool SubmitInterleaved(std::span<const std::int16_t>) override { return running; }
    bool running = false;
  };

  class FallbackFactory final : public ICorePlatformFactory
  {
  public:
    ISoundDriver *CreateSoundDriver() override { ++sound_calls; return nullptr; }
    Service::IFileops *CreateFileops(Service::ILog *) override { ++fileops_calls; return nullptr; }
    Service::IHud *CreateHud() override { ++hud_calls; return nullptr; }
    Service::IRetroPlatform *CreateRetroPlatform() override { ++retro_calls; return nullptr; }

    int sound_calls = 0;
    int fileops_calls = 0;
    int hud_calls = 0;
    int retro_calls = 0;
  };
}

int main()
{
  AudioOutput audio;
  FallbackFactory fallback;
  PortableAudioPlatformFactory factory(fallback, audio);

  ISoundDriver *sound = factory.CreateSoundDriver();
  if (sound == nullptr) return 1;
  if (fallback.sound_calls != 0) return 2;

  factory.CreateFileops(nullptr);
  factory.CreateHud();
  factory.CreateRetroPlatform();
  if (fallback.fileops_calls != 1 || fallback.hud_calls != 1 || fallback.retro_calls != 1) return 3;

  delete sound;
  return 0;
}
