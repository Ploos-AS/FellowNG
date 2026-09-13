#include "SdlAudioOutput.h"

#include <limits>

namespace FellowNG::Frontend::SDL
{
  SdlAudioOutput::~SdlAudioOutput()
  {
    Stop();
  }

  bool SdlAudioOutput::Start(const Platform::AudioFormat &format)
  {
    Stop();

    if (format.sample_rate == 0 || format.channels == 0)
    {
      return false;
    }

    if (format.sample_rate > static_cast<std::uint32_t>(std::numeric_limits<int>::max()) ||
        format.channels > static_cast<std::uint16_t>(std::numeric_limits<int>::max()))
    {
      return false;
    }

    SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_S16;
    spec.channels = static_cast<int>(format.channels);
    spec.freq = static_cast<int>(format.sample_rate);

    _stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
      &spec,
      nullptr,
      nullptr);
    if (_stream == nullptr)
    {
      return false;
    }

    if (!SDL_ResumeAudioStreamDevice(_stream))
    {
      SDL_DestroyAudioStream(_stream);
      _stream = nullptr;
      return false;
    }

    return true;
  }

  void SdlAudioOutput::Stop()
  {
    if (_stream != nullptr)
    {
      SDL_DestroyAudioStream(_stream);
      _stream = nullptr;
    }
  }

  bool SdlAudioOutput::IsRunning() const
  {
    return _stream != nullptr;
  }

  bool SdlAudioOutput::SubmitInterleaved(std::span<const std::int16_t> samples)
  {
    if (_stream == nullptr)
    {
      return false;
    }

    if (samples.empty())
    {
      return true;
    }

    const std::size_t bytes = samples.size_bytes();
    if (bytes > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
      return false;
    }

    return SDL_PutAudioStreamData(
      _stream,
      samples.data(),
      static_cast<int>(bytes));
  }
}
