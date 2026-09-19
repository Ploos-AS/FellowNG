#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "Platform/IVideoOutput.h"

namespace FellowNG::Frontend::SDL
{
  class SdlVideoOutput final : public Platform::IVideoOutput
  {
  public:
    explicit SdlVideoOutput(SDL_Window *window);
    ~SdlVideoOutput() override;

    bool Start(std::uint32_t width, std::uint32_t height) override;
    void Stop() override;
    bool IsRunning() const override;
    bool Present(const Platform::VideoFrame &frame) override;

    std::uint64_t PresentedFrameCount() const { return _presented_frame_count; }
    std::uint64_t ChangedFrameCount() const { return _changed_frame_count; }
    std::uint64_t LastFrameSignature() const { return _last_frame_signature; }
    std::uint64_t NonBlackPixelCount() const;
    bool SaveLastFramePpm(const std::string &path) const;

  private:
    bool RecreateTexture(std::uint32_t width, std::uint32_t height);

    SDL_Window *_window = nullptr;
    SDL_Renderer *_renderer = nullptr;
    SDL_Texture *_texture = nullptr;
    std::uint32_t _width = 0;
    std::uint32_t _height = 0;
    std::uint64_t _presented_frame_count = 0;
    std::uint64_t _changed_frame_count = 0;
    std::uint64_t _last_frame_signature = 0;
    std::vector<std::uint8_t> _last_frame_pixels;
    std::uint32_t _last_frame_width = 0;
    std::uint32_t _last_frame_height = 0;
    std::uint32_t _last_frame_pitch = 0;
  };
}
