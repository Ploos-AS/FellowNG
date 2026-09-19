#include "SdlVideoOutput.h"

#include <fstream>

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
    _last_frame_width = frame.width;
    _last_frame_height = frame.height;
    _last_frame_pitch = frame.pitch_bytes;
    _last_frame_pixels.assign(data, data + static_cast<std::size_t>(frame.pitch_bytes) * frame.height);

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

  std::uint64_t SdlVideoOutput::NonBackgroundPixelCount() const
  {
    if (_last_frame_pixels.empty() || _last_frame_width == 0 || _last_frame_height == 0) return 0;
    std::uint64_t count = 0;
    for (std::uint32_t y = 0; y < _last_frame_height; ++y)
    {
      const auto *row = _last_frame_pixels.data() + static_cast<std::size_t>(y) * _last_frame_pitch;
      for (std::uint32_t x = 0; x < _last_frame_width; ++x)
      {
        const auto *pixel = row + static_cast<std::size_t>(x) * 4u;
        // XRGB8888 black is 0x00000000. Count only pixels that contain
        // visible RGB information in the actual final framebuffer.
        if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0) ++count;
      }
    }
    return count;
  }

  bool SdlVideoOutput::SaveLastFramePpm(const std::string &path) const
  {
    if (_last_frame_pixels.empty() || _last_frame_width == 0 || _last_frame_height == 0) return false;
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out << "P6\n" << _last_frame_width << " " << _last_frame_height << "\n255\n";
    for (std::uint32_t y = 0; y < _last_frame_height; ++y)
    {
      const auto *row = _last_frame_pixels.data() + static_cast<std::size_t>(y) * _last_frame_pitch;
      for (std::uint32_t x = 0; x < _last_frame_width; ++x)
      {
        const auto *pixel = row + static_cast<std::size_t>(x) * 4u;
        const char rgb[3] = {static_cast<char>(pixel[2]), static_cast<char>(pixel[1]), static_cast<char>(pixel[0])};
        out.write(rgb, 3);
      }
    }
    return static_cast<bool>(out);
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
