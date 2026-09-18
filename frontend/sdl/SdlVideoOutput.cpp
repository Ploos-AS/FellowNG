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

    std::uint64_t signature = 1469598103934665603ull;
    const auto *data = reinterpret_cast<const std::uint8_t *>(frame.pixels.data());
    const std::size_t size = frame.pixels.size();
    const std::size_t stride = size > 4096 ? size / 4096 : 1;
    for (std::size_t i = 0; i < size; i += stride)
    {
      signature ^= data[i];
      signature *= 1099511628211ull;
    }
    ++_presented_frame_count;
    if (_presented_frame_count == 1 || signature != _last_frame_signature) ++_changed_frame_count;
    _last_frame_signature = signature;

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
