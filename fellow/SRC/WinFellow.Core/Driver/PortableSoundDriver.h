#pragma once

#include <cstdint>
#include <vector>

#include "Driver/ISoundDriver.h"
#include "Platform/IAudioOutput.h"

namespace FellowNG::Driver
{
  class PortableSoundDriver final : public ISoundDriver
  {
  public:
    explicit PortableSoundDriver(Platform::IAudioOutput &output) : _output(output) {}

    void Play(int16_t *leftBuffer, int16_t *rightBuffer, uint32_t sampleCount) override;
    void PollBufferPosition() override;
    bool SetCurrentSoundDeviceVolume(int volume) override;

    bool EmulationStart(SoundDriverRuntimeConfiguration runtimeConfiguration) override;
    void EmulationStop() override;

    bool IsInitialized() override;

  private:
    std::int16_t ApplyVolume(std::int16_t sample) const;

    Platform::IAudioOutput &_output;
    std::vector<std::int16_t> _interleaved;
    std::uint16_t _channels = 2;
    int _volume = 100;
    bool _initialized = false;
  };
}
