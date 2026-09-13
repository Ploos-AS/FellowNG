#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace FellowNG::Platform
{
  struct AudioFormat
  {
    std::uint32_t sample_rate = 44100;
    std::uint16_t channels = 2;
  };

  class IAudioOutput
  {
  public:
    virtual ~IAudioOutput() = default;

    virtual bool Start(const AudioFormat &format) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual bool SubmitInterleaved(std::span<const std::int16_t> samples) = 0;
  };
}
