#include <cstdint>
#include <span>
#include <vector>

#include "Driver/PortableSoundDriver.h"

namespace
{
  class TestAudioOutput final : public FellowNG::Platform::IAudioOutput
  {
  public:
    bool Start(const FellowNG::Platform::AudioFormat &format) override
    {
      last_format = format;
      running = true;
      return true;
    }

    void Stop() override
    {
      running = false;
    }

    bool IsRunning() const override
    {
      return running;
    }

    bool SubmitInterleaved(std::span<const std::int16_t> samples) override
    {
      submitted.assign(samples.begin(), samples.end());
      return running;
    }

    FellowNG::Platform::AudioFormat last_format{};
    std::vector<std::int16_t> submitted;
    bool running = false;
  };
}

int main()
{
  TestAudioOutput output;
  FellowNG::Driver::PortableSoundDriver driver(output);

  SoundDriverRuntimeConfiguration config{};
  config.IsStereo = true;
  config.Is16Bits = true;
  config.Volume = 100;
  config.ActualSampleRate = 44100;

  if (!driver.EmulationStart(config)) return 1;
  if (!driver.IsInitialized()) return 2;
  if (output.last_format.sample_rate != 44100 || output.last_format.channels != 2) return 3;

  std::int16_t left[] = {100, -200, 300};
  std::int16_t right[] = {-100, 200, -300};
  driver.Play(left, right, 3);

  const std::vector<std::int16_t> expected{100, -100, -200, 200, 300, -300};
  if (output.submitted != expected) return 4;

  if (!driver.SetCurrentSoundDeviceVolume(50)) return 5;
  driver.Play(left, right, 1);
  const std::vector<std::int16_t> expected_half{50, -50};
  if (output.submitted != expected_half) return 6;

  driver.EmulationStop();
  if (driver.IsInitialized() || output.running) return 7;

  return 0;
}
