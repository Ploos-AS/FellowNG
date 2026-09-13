#pragma once

#include <cstdint>

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

  private:
    bool RecreateTexture(std::uint32_t width, std::uint32_t height);

    SDL_Window *_window = nullptr;
    SDL_Renderer *_renderer = nullptr;
    SDL_Texture *_texture = nullptr;
    std::uint32_t _width = 0;
    std::uint32_t _height = 0;
  };
}
