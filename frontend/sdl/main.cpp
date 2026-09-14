#include <SDL3/SDL.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <span>
#include <vector>

#include "SdlAudioOutput.h"
#include "SdlFrontendSession.h"
#include "SdlInputSource.h"
#include "SdlVideoOutput.h"

namespace
{
  constexpr int DefaultWidth = 720;
  constexpr int DefaultHeight = 568;
  constexpr std::uint32_t TestWidth = 320;
  constexpr std::uint32_t TestHeight = 256;

  class SessionSelfTestRuntime final : public FellowNG::Platform::IEmulatorRuntime
  {
  public:
    bool Start(FellowNG::Platform::IVideoOutput &, FellowNG::Platform::IAudioOutput &) override
    {
      running = true;
      ++start_count;
      return true;
    }

    void Stop() override
    {
      if (running) ++stop_count;
      running = false;
    }

    bool IsRunning() const override { return running; }

    void HandleInput(const FellowNG::Platform::InputEvent &event) override
    {
      if (event.type == FellowNG::Platform::InputType::Key) ++key_count;
    }

    bool RunSlice() override
    {
      ++slice_count;
      return running;
    }

    bool running = false;
    int start_count = 0;
    int stop_count = 0;
    int key_count = 0;
    int slice_count = 0;
  };

  void DrainSdlEventQueue()
  {
    SDL_Event event{};
    while (SDL_PollEvent(&event))
    {
    }
  }

  std::vector<std::uint32_t> BuildTestFrame()
  {
    std::vector<std::uint32_t> pixels(TestWidth * TestHeight);
    for (std::uint32_t y = 0; y < TestHeight; ++y)
      for (std::uint32_t x = 0; x < TestWidth; ++x)
      {
        const std::uint32_t r = (x * 255u) / (TestWidth - 1u);
        const std::uint32_t g = (y * 255u) / (TestHeight - 1u);
        const std::uint32_t b = ((x ^ y) & 0xffu);
        pixels[y * TestWidth + x] = (r << 16u) | (g << 8u) | b;
      }
    return pixels;
  }

  bool PushInputSelfTestEvents()
  {
    SDL_Event key{};
    key.type = SDL_EVENT_KEY_DOWN;
    key.key.scancode = SDL_SCANCODE_A;
    if (!SDL_PushEvent(&key)) return false;

    SDL_Event mouse{};
    mouse.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    mouse.button.button = SDL_BUTTON_LEFT;
    if (!SDL_PushEvent(&mouse)) return false;

    SDL_Event gamepad{};
    gamepad.type = SDL_EVENT_GAMEPAD_AXIS_MOTION;
    gamepad.gaxis.axis = SDL_GAMEPAD_AXIS_LEFTX;
    gamepad.gaxis.value = 1234;
    return SDL_PushEvent(&gamepad);
  }

  bool RunInputSelfTest(FellowNG::Frontend::SDL::SdlInputSource &input)
  {
    if (!PushInputSelfTestEvents()) return false;

    const auto key = input.Poll();
    if (!key || key->type != FellowNG::Platform::InputType::Key ||
        key->code != static_cast<std::int32_t>(FellowNG::Platform::KeyCode::A) || !key->pressed)
      return false;

    const auto mouse = input.Poll();
    if (!mouse || mouse->type != FellowNG::Platform::InputType::MouseButton ||
        mouse->code != static_cast<std::int32_t>(FellowNG::Platform::MouseButton::Left) || !mouse->pressed)
      return false;

    const auto gamepad = input.Poll();
    return gamepad && gamepad->type == FellowNG::Platform::InputType::JoystickAxis &&
           gamepad->code == static_cast<std::int32_t>(FellowNG::Platform::JoystickAxis::LeftX) &&
           gamepad->value == 1234;
  }

  bool RunAudioSelfTest(FellowNG::Frontend::SDL::SdlAudioOutput &audio)
  {
    const FellowNG::Platform::AudioFormat format{.sample_rate = 44100, .channels = 2};
    if (!audio.Start(format) || !audio.IsRunning()) return false;
    const std::vector<std::int16_t> silence(512u * format.channels, 0);
    const bool submitted = audio.SubmitInterleaved(silence);
    audio.Stop();
    return submitted && !audio.IsRunning();
  }

  bool RunSessionSelfTest(FellowNG::Frontend::SDL::SdlInputSource &input,
                          FellowNG::Frontend::SDL::SdlVideoOutput &video,
                          FellowNG::Frontend::SDL::SdlAudioOutput &audio)
  {
    // SDL owns a process-wide event queue. Previous video/input/audio probes may
    // leave window or device events behind, so isolate this end-to-end session
    // test before injecting its deterministic key/quit sequence.
    DrainSdlEventQueue();

    SessionSelfTestRuntime runtime;
    FellowNG::Frontend::SDL::SdlFrontendSession session(runtime, input, video, audio);
    if (!session.Start() || !session.IsRunning())
    {
      std::cerr << "runtime-session: start failed\n";
      return false;
    }

    SDL_Event key{};
    key.type = SDL_EVENT_KEY_DOWN;
    key.key.scancode = SDL_SCANCODE_B;
    if (!SDL_PushEvent(&key))
    {
      std::cerr << "runtime-session: SDL_PushEvent(key) failed: " << SDL_GetError() << '\n';
      return false;
    }

    if (!session.PumpOnce())
    {
      std::cerr << "runtime-session: key pump stopped unexpectedly\n";
      return false;
    }
    if (runtime.start_count != 1 || runtime.key_count != 1 || runtime.slice_count != 1)
    {
      std::cerr << "runtime-session: counters after key: start=" << runtime.start_count
                << " key=" << runtime.key_count << " slice=" << runtime.slice_count << '\n';
      return false;
    }

    SDL_Event quit{};
    quit.type = SDL_EVENT_QUIT;
    if (!SDL_PushEvent(&quit))
    {
      std::cerr << "runtime-session: SDL_PushEvent(quit) failed: " << SDL_GetError() << '\n';
      return false;
    }

    if (session.PumpOnce())
    {
      std::cerr << "runtime-session: quit pump stayed running\n";
      return false;
    }
    if (session.IsRunning() || runtime.running || runtime.stop_count != 1)
    {
      std::cerr << "runtime-session: stop state invalid: session=" << session.IsRunning()
                << " runtime=" << runtime.running << " stop=" << runtime.stop_count << '\n';
      return false;
    }
    return true;
  }
}

int main(int argc, char **argv)
{
  const bool self_test = argc > 1 && std::strcmp(argv[1], "--self-test") == 0;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
  {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
  if (self_test) flags |= SDL_WINDOW_HIDDEN;

  SDL_Window *window = SDL_CreateWindow("FellowNG SDL3", DefaultWidth, DefaultHeight, flags);
  if (window == nullptr)
  {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
    SDL_Quit();
    return EXIT_FAILURE;
  }

  FellowNG::Frontend::SDL::SdlVideoOutput video(window);
  FellowNG::Frontend::SDL::SdlInputSource input;
  FellowNG::Frontend::SDL::SdlAudioOutput audio;

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
    const bool audio_ok = RunAudioSelfTest(audio);
    const bool session_ok = RunSessionSelfTest(input, video, audio);
    video.Stop();
    SDL_DestroyWindow(window);
    SDL_Quit();
    if (!input_ok)
    {
      std::cerr << "FellowNG SDL3 input backend self-test failed\n";
      return EXIT_FAILURE;
    }
    if (!audio_ok)
    {
      std::cerr << "FellowNG SDL3 audio backend self-test failed\n";
      return EXIT_FAILURE;
    }
    if (!session_ok)
    {
      std::cerr << "FellowNG SDL3 runtime-session self-test failed\n";
      return EXIT_FAILURE;
    }
    std::cout << "FellowNG SDL3 video+input+audio+runtime-session: PASS\n";
    return EXIT_SUCCESS;
  }

  bool running = true;
  while (running)
  {
    while (const auto event = input.Poll())
      if (event->type == FellowNG::Platform::InputType::Quit) running = false;
    SDL_Delay(1);
  }

  audio.Stop();
  video.Stop();
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
