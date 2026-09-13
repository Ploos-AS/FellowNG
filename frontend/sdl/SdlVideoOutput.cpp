#include "SdlVideoOutput.h"

namespace FellowNG::Frontend::SDL
{
  SdlVideoOutput::SdlVideoOutput(SDL_Window *window)
    : _window(window)
  {
  }

  SdlVideoOutput::~SdlVideoOutput()
  {
    Stop();
  }

  bool SdlVideoOutput::Start(std::uint32_t width, std::uint32_t height)
  {
    if (_window == nullptr || width == 0 || height == 0)
    {
      return false;
    }

    if (_renderer == nullptr)
    {
      _renderer = SDL_CreateRenderer(_window, nullptr);
      if (_renderer == nullptr)
      {
        return false;
      }
    }

    return RecreateTexture(width, height);
  }

  void SdlVideoOutput::Stop()
  {
    if (_texture != nullptr)
    {
      SDL_DestroyTexture(_texture);
      _texture = nullptr;
    }

    if (_renderer != nullptr)
    {
      SDL_DestroyRenderer(_renderer);
      _renderer = nullptr;
    }

    _width = 0;
    _height = 0;
  }

  bool SdlVideoOutput::IsRunning() const
  {
    return _renderer != nullptr && _texture != nullptr;
  }

  bool SdlVideoOutput::Present(const Platform::VideoFrame &frame)
  {
    if (_renderer == nullptr || frame.format != Platform::PixelFormat::Xrgb8888 ||
        frame.width == 0 || frame.height == 0 || frame.pitch_bytes < frame.width * 4 ||
        frame.pixels.size() < static_cast<std::size_t>(frame.pitch_bytes) * frame.height)
    {
      return false;
    }

    if (_texture == nullptr || frame.width != _width || frame.height != _height)
    {
      if (!RecreateTexture(frame.width, frame.height))
      {
        return false;
      }
    }

    if (!SDL_UpdateTexture(_texture, nullptr, frame.pixels.data(), static_cast<int>(frame.pitch_bytes)))
    {
      return false;
    }

    if (!SDL_RenderClear(_renderer))
    {
      return false;
    }

    if (!SDL_RenderTexture(_renderer, _texture, nullptr, nullptr))
    {
      return false;
    }

    return SDL_RenderPresent(_renderer);
  }

  bool SdlVideoOutput::RecreateTexture(std::uint32_t width, std::uint32_t height)
  {
    if (_renderer == nullptr)
    {
      return false;
    }

    if (_texture != nullptr)
    {
      SDL_DestroyTexture(_texture);
      _texture = nullptr;
    }

    _texture = SDL_CreateTexture(
      _renderer,
      SDL_PIXELFORMAT_XRGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      static_cast<int>(width),
      static_cast<int>(height));

    if (_texture == nullptr)
    {
      _width = 0;
      _height = 0;
      return false;
    }

    SDL_SetTextureScaleMode(_texture, SDL_SCALEMODE_NEAREST);
    _width = width;
    _height = height;
    return true;
  }
}
