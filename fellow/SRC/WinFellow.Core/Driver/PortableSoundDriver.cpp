#include "Driver/PortableSoundDriver.h"

#include <algorithm>
#include <cstdint>

namespace FellowNG::Driver
{
  void PortableSoundDriver::Play(int16_t *leftBuffer, int16_t *rightBuffer, uint32_t sampleCount)
  {
    if (!_initialized || sampleCount == 0 || leftBuffer == nullptr) return;

    if (_channels == 2)
    {
      if (rightBuffer == nullptr) return;
      _interleaved.resize(static_cast<std::size_t>(sampleCount) * 2);
      for (std::uint32_t i = 0; i < sampleCount; ++i)
      {
        _interleaved[(static_cast<std::size_t>(i) * 2)] = ApplyVolume(leftBuffer[i]);
        _interleaved[(static_cast<std::size_t>(i) * 2) + 1] = ApplyVolume(rightBuffer[i]);
      }
    }
    else
    {
      _interleaved.resize(sampleCount);
      for (std::uint32_t i = 0; i < sampleCount; ++i)
      {
        if (rightBuffer != nullptr)
        {
          const auto mixed = (static_cast<std::int32_t>(leftBuffer[i]) + static_cast<std::int32_t>(rightBuffer[i])) / 2;
          _interleaved[i] = ApplyVolume(static_cast<std::int16_t>(mixed));
        }
        else
        {
          _interleaved[i] = ApplyVolume(leftBuffer[i]);
        }
      }
    }

    _output.SubmitInterleaved(_interleaved);
  }

  void PortableSoundDriver::PollBufferPosition()
  {
    // Push-based portable audio has no device cursor to poll.
  }

  bool PortableSoundDriver::SetCurrentSoundDeviceVolume(int volume)
  {
    _volume = std::clamp(volume, 0, 100);
    return true;
  }

  bool PortableSoundDriver::EmulationStart(SoundDriverRuntimeConfiguration runtimeConfiguration)
  {
    if (_initialized) return true;
    if (!runtimeConfiguration.Is16Bits || runtimeConfiguration.ActualSampleRate == 0) return false;

    _channels = runtimeConfiguration.IsStereo ? 2 : 1;
    _volume = std::clamp(runtimeConfiguration.Volume, 0, 100);

    const Platform::AudioFormat format{
        .sample_rate = runtimeConfiguration.ActualSampleRate,
        .channels = _channels,
    };

    _initialized = _output.Start(format);
    return _initialized;
  }

  void PortableSoundDriver::EmulationStop()
  {
    if (_initialized) _output.Stop();
    _initialized = false;
    _interleaved.clear();
  }

  bool PortableSoundDriver::IsInitialized()
  {
    return _initialized && _output.IsRunning();
  }

  std::int16_t PortableSoundDriver::ApplyVolume(std::int16_t sample) const
  {
    const auto scaled = (static_cast<std::int32_t>(sample) * _volume) / 100;
    return static_cast<std::int16_t>(scaled);
  }
}
