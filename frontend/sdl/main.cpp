#include <SDL3/SDL.h>
#include "FilesystemIntegration.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

#include "SdlAudioOutput.h"
#include "SdlFrontendSession.h"
#include "SdlInputSource.h"
#include "SdlVideoOutput.h"
#include "WinFellowRuntimeFactory.h"

namespace
{
  constexpr int DefaultWidth = 720;
  constexpr int DefaultHeight = 568;
  constexpr std::uint32_t TestWidth = 320;
  constexpr std::uint32_t TestHeight = 256;
  constexpr int RuntimeSlicesPerPump = 4096;

  class SessionSelfTestRuntime final : public FellowNG::Platform::IEmulatorRuntime
  {
  public:
    bool Start(FellowNG::Platform::IVideoOutput &, FellowNG::Platform::IAudioOutput &) override
    {
      running = true;
      ++start_count;
      return true;
    }
    void Stop() override { if (running) ++stop_count; running = false; }
    bool IsRunning() const override { return running; }
    void HandleInput(const FellowNG::Platform::InputEvent &event) override
    { if (event.type == FellowNG::Platform::InputType::Key) ++key_count; }
    bool RunSlice() override { ++slice_count; return running; }
    bool running = false;
    int start_count = 0, stop_count = 0, key_count = 0, slice_count = 0;
  };

  void DrainSdlEventQueue()
  {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {}
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
    SDL_Event key{}; key.type = SDL_EVENT_KEY_DOWN; key.key.scancode = SDL_SCANCODE_A;
    if (!SDL_PushEvent(&key)) { std::cerr << "input: push key failed: " << SDL_GetError() << '\n'; return false; }
    SDL_Event mouse{}; mouse.type = SDL_EVENT_MOUSE_BUTTON_DOWN; mouse.button.button = SDL_BUTTON_LEFT;
    if (!SDL_PushEvent(&mouse)) { std::cerr << "input: push mouse failed: " << SDL_GetError() << '\n'; return false; }
    SDL_Event gamepad{}; gamepad.type = SDL_EVENT_GAMEPAD_AXIS_MOTION; gamepad.gaxis.axis = SDL_GAMEPAD_AXIS_LEFTX; gamepad.gaxis.value = 1234;
    if (!SDL_PushEvent(&gamepad)) { std::cerr << "input: push gamepad failed: " << SDL_GetError() << '\n'; return false; }
    return true;
  }

  bool RunInputSelfTest(FellowNG::Frontend::SDL::SdlInputSource &input)
  {
    DrainSdlEventQueue();
    if (!PushInputSelfTestEvents()) return false;
    const auto key = input.Poll();
    if (!key || key->type != FellowNG::Platform::InputType::Key || key->code != static_cast<std::int32_t>(FellowNG::Platform::KeyCode::A) || !key->pressed)
    { std::cerr << "input: key translation failed\n"; return false; }
    const auto mouse = input.Poll();
    if (!mouse || mouse->type != FellowNG::Platform::InputType::MouseButton || mouse->code != static_cast<std::int32_t>(FellowNG::Platform::MouseButton::Left) || !mouse->pressed)
    { std::cerr << "input: mouse translation failed\n"; return false; }
    const auto gamepad = input.Poll();
    if (!gamepad || gamepad->type != FellowNG::Platform::InputType::JoystickAxis || gamepad->code != static_cast<std::int32_t>(FellowNG::Platform::JoystickAxis::LeftX) || gamepad->value != 1234)
    { std::cerr << "input: gamepad translation failed\n"; return false; }
    return true;
  }

  bool RunAudioSelfTest(FellowNG::Frontend::SDL::SdlAudioOutput &audio)
  {
    const FellowNG::Platform::AudioFormat format{.sample_rate = 44100, .channels = 2};
    if (!audio.Start(format)) { std::cerr << "audio: start failed: " << SDL_GetError() << '\n'; return false; }
    if (!audio.IsRunning()) { std::cerr << "audio: stream not running after start\n"; return false; }
    const std::vector<std::int16_t> silence(512u * format.channels, 0);
    if (!audio.SubmitInterleaved(silence)) { std::cerr << "audio: submit failed: " << SDL_GetError() << '\n'; audio.Stop(); return false; }
    audio.Stop();
    if (audio.IsRunning()) { std::cerr << "audio: stream still running after stop\n"; return false; }
    return true;
  }

  bool RunSessionSelfTest(FellowNG::Frontend::SDL::SdlInputSource &input, FellowNG::Frontend::SDL::SdlVideoOutput &video, FellowNG::Frontend::SDL::SdlAudioOutput &audio)
  {
    DrainSdlEventQueue();
    SessionSelfTestRuntime runtime;
    FellowNG::Frontend::SDL::SdlFrontendSession session(runtime, input, video, audio);
    if (!session.Start() || !session.IsRunning()) { std::cerr << "runtime-session: start failed\n"; return false; }
    SDL_Event key{}; key.type = SDL_EVENT_KEY_DOWN; key.key.scancode = SDL_SCANCODE_B;
    if (!SDL_PushEvent(&key)) { std::cerr << "runtime-session: SDL_PushEvent(key) failed: " << SDL_GetError() << '\n'; return false; }
    if (!session.PumpOnce()) { std::cerr << "runtime-session: key pump stopped unexpectedly\n"; return false; }
    if (runtime.start_count != 1 || runtime.key_count != 1 || runtime.slice_count != RuntimeSlicesPerPump)
    { std::cerr << "runtime-session: counters after key: start=" << runtime.start_count << " key=" << runtime.key_count << " slice=" << runtime.slice_count << '\n'; return false; }
    SDL_Event quit{}; quit.type = SDL_EVENT_QUIT;
    if (!SDL_PushEvent(&quit)) { std::cerr << "runtime-session: SDL_PushEvent(quit) failed: " << SDL_GetError() << '\n'; return false; }
    if (session.PumpOnce()) { std::cerr << "runtime-session: quit pump stayed running\n"; return false; }
    if (session.IsRunning() || runtime.running || runtime.stop_count != 1)
    { std::cerr << "runtime-session: stop state invalid: session=" << session.IsRunning() << " runtime=" << runtime.running << " stop=" << runtime.stop_count << '\n'; return false; }
    return true;
  }
}

int main(int argc, char **argv)
{
  const bool self_test = argc > 1 && std::strcmp(argv[1], "--self-test") == 0;
  const bool runtime_smoke = argc > 1 && std::strcmp(argv[1], "--runtime-smoke") == 0;
  const bool runtime_boot = argc > 1 && std::strcmp(argv[1], "--runtime-boot") == 0;
  const bool runtime_boot_deep = argc > 1 && std::strcmp(argv[1], "--runtime-boot-deep") == 0;
  const bool runtime_boot_desktop = argc > 1 && std::strcmp(argv[1], "--runtime-boot-desktop") == 0;
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
  { std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n'; return EXIT_FAILURE; }
  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
  if (self_test || runtime_smoke || runtime_boot || runtime_boot_deep || runtime_boot_desktop) flags |= SDL_WINDOW_HIDDEN;
  SDL_Window *window = SDL_CreateWindow("FellowNG SDL3", DefaultWidth, DefaultHeight, flags);
  if (window == nullptr) { std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n'; SDL_Quit(); return EXIT_FAILURE; }
  FellowNG::Frontend::SDL::SdlVideoOutput video(window);
  FellowNG::Frontend::SDL::SdlInputSource input;
  FellowNG::Frontend::SDL::SdlAudioOutput audio;
  if (!video.Start(TestWidth, TestHeight)) { std::cerr << "SDL video start failed: " << SDL_GetError() << '\n'; SDL_DestroyWindow(window); SDL_Quit(); return EXIT_FAILURE; }
  const auto pixels = BuildTestFrame();
  const auto bytes = std::as_bytes(std::span<const std::uint32_t>(pixels));
  FellowNG::Platform::VideoFrame frame{.width = TestWidth, .height = TestHeight, .pitch_bytes = TestWidth * 4u, .format = FellowNG::Platform::PixelFormat::Xrgb8888, .pixels = bytes};
  if (!video.Present(frame)) { std::cerr << "SDL video present failed: " << SDL_GetError() << '\n'; video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return EXIT_FAILURE; }

  if (self_test)
  {
    std::cout << "self-test: video PASS\n" << std::flush;
    if (!RunInputSelfTest(input)) { std::cerr << "self-test: input FAIL\n"; video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 11; }
    std::cout << "self-test: input PASS\n" << std::flush;
    if (!RunAudioSelfTest(audio)) { std::cerr << "self-test: audio FAIL\n"; video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 12; }
    std::cout << "self-test: audio PASS\n" << std::flush;
    if (!RunSessionSelfTest(input, video, audio)) { std::cerr << "self-test: runtime-session FAIL\n"; video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 13; }
    std::cout << "self-test: runtime-session PASS\n" << std::flush;
    video.Stop(); SDL_DestroyWindow(window); SDL_Quit();
    std::cout << "FellowNG SDL3 video+input+audio+runtime-session: PASS\n";
    return EXIT_SUCCESS;
  }

  // Normal execution owns the real Fellow runtime through the portable
  // runtime-factory/session seam. ROM and AmigaOS paths remain ordinary
  // user-supplied Fellow command-line/configuration inputs.
  std::vector<const char *> runtime_argv;
  runtime_argv.reserve(static_cast<std::size_t>(argc));
  runtime_argv.push_back(argv[0]);
  for (int i = (runtime_smoke || runtime_boot || runtime_boot_deep || runtime_boot_desktop) ? 2 : 1; i < argc; ++i) runtime_argv.push_back(argv[i]);
  const int runtime_argc = static_cast<int>(runtime_argv.size());

  FellowNG::Runtime::WinFellowRuntimeFactory runtime_factory(
      runtime_argc, runtime_argv.empty() ? nullptr : runtime_argv.data());
  FellowNG::Frontend::SDL::SdlFrontendSession session(runtime_factory, input, video, audio);

  if (!session.HasRuntime())
  {
    std::cerr << "Fellow runtime factory failed to create a runtime\n";
    video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 20;
  }
  if (!session.Start())
  {
    if (runtime_smoke)
    {
      std::cout << "FellowNG SDL3 runtime-smoke: expected startup rejection without external ROM PASS\n";
      video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return EXIT_SUCCESS;
    }
    std::cerr << "Fellow runtime failed to start; check the external ROM/configuration inputs\n";
    video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 21;
  }

  if (runtime_boot_desktop) ffilesysDumpConfig();

  if (runtime_boot || runtime_boot_deep || runtime_boot_desktop)
  {
    const int BootPumps = runtime_boot_desktop ? 16384 : (runtime_boot_deep ? 4096 : 256);
    const std::uint64_t DesktopChangedFrameTarget = 16u;
    const std::uint64_t DesktopVisiblePixelTarget = 1024u;
    int completed_pumps = 0;
    for (; completed_pumps < BootPumps && session.IsRunning();)
    {
      if (!session.PumpOnce()) break;
      ++completed_pumps;
      if (runtime_boot_desktop &&
          video.ChangedFrameCount() >= DesktopChangedFrameTarget &&
          video.NonBackgroundPixelCount() >= DesktopVisiblePixelTarget)
      {
        if (!video.SaveLastFramePpm("fellowng-desktop-evidence.ppm"))
        {
          std::cerr << "runtime-boot: failed to save visible desktop framebuffer evidence\n";
          session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 25;
        }
        break;
      }
    }
    const bool desktop_progress_reached = runtime_boot_desktop &&
      video.ChangedFrameCount() >= DesktopChangedFrameTarget &&
      video.NonBackgroundPixelCount() >= DesktopVisiblePixelTarget;
    if ((!runtime_boot_desktop && completed_pumps != BootPumps) || (runtime_boot_desktop && !desktop_progress_reached) || !session.IsRunning())
    {
      if (runtime_boot_desktop && !video.SaveLastFramePpm("fellowng-desktop-evidence.ppm"))
      {
        std::cerr << "runtime-boot: failed to save diagnostic framebuffer evidence\n";
      }
      std::cerr << "runtime-boot: runtime stopped or progress target missed after " << completed_pumps << "/" << BootPumps
                << " pumps frames=" << video.PresentedFrameCount()
                << " changed=" << video.ChangedFrameCount()
                << " signature=" << video.LastFrameSignature()
                << " target=" << DesktopChangedFrameTarget
                << " visible_pixels=" << video.NonBackgroundPixelCount()
                << " visible_target=" << DesktopVisiblePixelTarget << "\n";
      session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 23;
    }
    const auto presented_frames = video.PresentedFrameCount();
    const auto changed_frames = video.ChangedFrameCount();
    const auto last_signature = video.LastFrameSignature();
    const auto minimum_changed_frames = runtime_boot_desktop ? DesktopChangedFrameTarget : (runtime_boot_deep ? 4u : 1u);
    if (presented_frames == 0 || changed_frames < minimum_changed_frames)
    {
      std::cerr << "runtime-boot: insufficient framebuffer progress frames=" << presented_frames
                << " changed=" << changed_frames << " required_changed=" << minimum_changed_frames << "\n";
      session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 24;
    }
    if ((runtime_boot_deep || runtime_boot_desktop) && !video.SaveLastFramePpm(runtime_boot_desktop ? "fellowng-desktop-evidence.ppm" : "fellowng-boot-evidence.ppm"))
    {
      std::cerr << "runtime-boot: failed to save final framebuffer evidence\n";
      session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 25;
    }
    session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit();
    const char *boot_mode = runtime_boot_desktop ? "runtime-boot-desktop" : (runtime_boot_deep ? "runtime-boot-deep" : "runtime-boot");
    std::cout << "FellowNG SDL3 " << boot_mode
              << ": sustained execution PASS pumps=" << completed_pumps
              << " slices=" << (completed_pumps * RuntimeSlicesPerPump)
              << " frames=" << presented_frames << " changed=" << changed_frames
              << " signature=" << last_signature << "\n";
    return EXIT_SUCCESS;
  }

  if (runtime_smoke)
  {
    if (!session.PumpOnce())
    {
      std::cerr << "runtime-smoke: runtime stopped during first bounded pump\n";
      session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return 22;
    }
    session.Stop(); audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit();
    std::cout << "FellowNG SDL3 runtime-smoke: real runtime start+pump+stop PASS\n";
    return EXIT_SUCCESS;
  }

  while (session.IsRunning() && session.PumpOnce()) SDL_Delay(1);

  session.Stop();
  audio.Stop(); video.Stop(); SDL_DestroyWindow(window); SDL_Quit(); return EXIT_SUCCESS;
}
