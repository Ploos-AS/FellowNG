#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace FellowNG::Platform
{
  enum class PixelFormat
  {
    Xrgb8888
  };

  struct VideoFrame
  {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t pitch_bytes = 0;
    PixelFormat format = PixelFormat::Xrgb8888;
    std::span<const std::byte> pixels{};
  };

  class IVideoOutput
  {
  public:
    virtual ~IVideoOutput() = default;

    virtual bool Start(std::uint32_t width, std::uint32_t height) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual bool Present(const VideoFrame &frame) = 0;
  };
}
