#pragma once

#include <SDL3/SDL.h>

#include "../../fellow/SRC/WinFellow.Core/Platform/IAudioOutput.h"

namespace FellowNG::Frontend::SDL
{
  class SdlAudioOutput final : public Platform::IAudioOutput
  {
  public:
    SdlAudioOutput() = default;
    ~SdlAudioOutput() override;

    SdlAudioOutput(const SdlAudioOutput &) = delete;
    SdlAudioOutput &operator=(const SdlAudioOutput &) = delete;

    bool Start(const Platform::AudioFormat &format) override;
    void Stop() override;
    bool IsRunning() const override;
    bool SubmitInterleaved(std::span<const std::int16_t> samples) override;

  private:
    SDL_AudioStream *_stream = nullptr;
  };
}
