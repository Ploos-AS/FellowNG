#include <SDL3/SDL.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <span>
#include <vector>

#include "SdlInputSource.h"
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

  bool PushInputSelfTestEvents()
  {
    SDL_Event key{};
    key.type = SDL_EVENT_KEY_DOWN;
    key.key.scancode = SDL_SCANCODE_A;
    key.key.key = SDLK_A;
    if (!SDL_PushEvent(&key)) return false;

    SDL_Event mouse{};
    mouse.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    mouse.button.button = SDL_BUTTON_LEFT;
    if (!SDL_PushEvent(&mouse)) return false;

    SDL_Event gamepad{};
    gamepad.type = SDL_EVENT_GAMEPAD_AXIS_MOTION;
    gamepad.gaxis.axis = SDL_GAMEPAD_AXIS_LEFTX;
    gamepad.gaxis.value = 1234;
    gamepad.gaxis.which = 7;
    if (!SDL_PushEvent(&gamepad)) return false;

    SDL_Event quit{};
    quit.type = SDL_EVENT_QUIT;
    return SDL_PushEvent(&quit);
  }

  bool RunInputSelfTest(FellowNG::Frontend::SDL::SdlInputSource &input)
  {
    if (!PushInputSelfTestEvents()) return false;

    const auto key = input.Poll();
    if (!key || key->type != FellowNG::Platform::InputType::Key ||
        key->code != SDL_SCANCODE_A || !key->pressed)
      return false;

    const auto mouse = input.Poll();
    if (!mouse || mouse->type != FellowNG::Platform::InputType::MouseButton ||
        mouse->code != SDL_BUTTON_LEFT || !mouse->pressed)
      return false;

    const auto gamepad = input.Poll();
    if (!gamepad || gamepad->type != FellowNG::Platform::InputType::JoystickAxis ||
        gamepad->code != SDL_GAMEPAD_AXIS_LEFTX || gamepad->value != 1234)
      return false;

    const auto quit = input.Poll();
    return quit && quit->type == FellowNG::Platform::InputType::Quit;
  }
}

int main(int argc, char **argv)
{
  const bool self_test = argc > 1 && std::strcmp(argv[1], "--self-test") == 0;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
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
  FellowNG::Frontend::SDL::SdlInputSource input;

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
    const bool input_ok = RunInputSelfTest(input);
    video.Stop();
    SDL_DestroyWindow(window);
    SDL_Quit();
    if (!input_ok)
    {
      std::cerr << "FellowNG SDL3 input backend self-test failed\n";
      return EXIT_FAILURE;
    }
    std::cout << "FellowNG SDL3 video+input backends: PASS\n";
    return EXIT_SUCCESS;
  }

  bool running = true;
  while (running)
  {
    while (const auto event = input.Poll())
    {
      if (event->type == FellowNG::Platform::InputType::Quit)
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
