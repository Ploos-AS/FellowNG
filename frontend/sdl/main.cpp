#include <SDL3/SDL.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <span>
#include <vector>

#include "SdlVideoOutput.h"

namespace
{
  constexpr int DefaultWidth = 720;
  constexpr int DefaultHeight = 568;
  constexpr std::uint32_t TestWidth = 320;
  constexpr std::uint32_t TestHeight = 256;

  std::vector<std::uint32_t> BuildTestFrame()
  {
    std::vector<std::uint32_t> pixels(TestWidth * TestHeight);
    for (std::uint32_t y = 0; y < TestHeight; ++y)
    {
      for (std::uint32_t x = 0; x < TestWidth; ++x)
      {
        const std::uint32_t r = (x * 255u) / (TestWidth - 1u);
        const std::uint32_t g = (y * 255u) / (TestHeight - 1u);
        const std::uint32_t b = ((x ^ y) & 0xffu);
        pixels[y * TestWidth + x] = (r << 16u) | (g << 8u) | b;
      }
    }
    return pixels;
  }
}

int main(int argc, char **argv)
{
  const bool self_test = argc > 1 && std::strcmp(argv[1], "--self-test") == 0;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
  {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
  if (self_test)
  {
    flags |= SDL_WINDOW_HIDDEN;
  }

  SDL_Window *window = SDL_CreateWindow("FellowNG SDL3", DefaultWidth, DefaultHeight, flags);
  if (window == nullptr)
  {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
    SDL_Quit();
    return EXIT_FAILURE;
  }

  FellowNG::Frontend::SDL::SdlVideoOutput video(window);
  if (!video.Start(TestWidth, TestHeight))
  {
    std::cerr << "SDL video start failed: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  const auto pixels = BuildTestFrame();
  const auto bytes = std::as_bytes(std::span<const std::uint32_t>(pixels));
  FellowNG::Platform::VideoFrame frame{
    .width = TestWidth,
    .height = TestHeight,
    .pitch_bytes = TestWidth * 4u,
    .format = FellowNG::Platform::PixelFormat::Xrgb8888,
    .pixels = bytes,
  };

  if (!video.Present(frame))
  {
    std::cerr << "SDL video present failed: " << SDL_GetError() << '\n';
    video.Stop();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (self_test)
  {
    video.Stop();
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "FellowNG SDL3 video backend: PASS\n";
    return EXIT_SUCCESS;
  }

  bool running = true;
  while (running)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        running = false;
      }
    }

    SDL_Delay(1);
  }

  video.Stop();
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
