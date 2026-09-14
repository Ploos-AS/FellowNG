#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <span>

#include "Platform/FrontendSession.h"

namespace
{
  class NullAudio final : public FellowNG::Platform::IAudioOutput
  {
  public:
    bool Start(const FellowNG::Platform::AudioFormat &) override { running = true; return true; }
    void Stop() override { running = false; }
    bool IsRunning() const override { return running; }
    bool SubmitInterleaved(std::span<const std::int16_t>) override { return running; }
    bool running = false;
  };

  class NullVideo final : public FellowNG::Platform::IVideoOutput
  {
  public:
    bool Start(std::uint32_t, std::uint32_t) override { running = true; return true; }
    void Stop() override { running = false; }
    bool IsRunning() const override { return running; }
    bool Present(const FellowNG::Platform::VideoFrame &) override { return running; }
    bool running = false;
  };

  class ScriptedInput final : public FellowNG::Platform::IInputSource
  {
  public:
    std::optional<FellowNG::Platform::InputEvent> Poll() override
    {
      if (stage == 0)
      {
        ++stage;
        return FellowNG::Platform::InputEvent{.type = FellowNG::Platform::InputType::Key, .code = 17, .pressed = true};
      }
      return std::nullopt;
    }

    void QueueQuit() { stage = 2; }

    int stage = 0;
  };

  class TestRuntime final : public FellowNG::Platform::IEmulatorRuntime
  {
  public:
    bool Start(FellowNG::Platform::IVideoOutput &, FellowNG::Platform::IAudioOutput &) override
    {
      running = true;
      return true;
    }

    void Stop() override
    {
      running = false;
      ++stop_count;
    }

    bool IsRunning() const override { return running; }

    void HandleInput(const FellowNG::Platform::InputEvent &event) override
    {
      last_code = event.code;
      ++input_count;
    }

    bool RunSlice() override
    {
      ++slice_count;
      return true;
    }

    bool running = false;
    int input_count = 0;
    int slice_count = 0;
    int stop_count = 0;
    std::int32_t last_code = 0;
  };
}

int main()
{
  constexpr int ExpectedSlicesPerPump = 4096;

  TestRuntime runtime;
  ScriptedInput input;
  NullVideo video;
  NullAudio audio;
  FellowNG::Platform::FrontendSession session(runtime, input, video, audio);

  if (!session.Start() || !session.IsRunning()) return EXIT_FAILURE;
  if (!session.PumpOnce()) return EXIT_FAILURE;
  if (runtime.input_count != 1 || runtime.last_code != 17) return EXIT_FAILURE;
  if (runtime.slice_count != ExpectedSlicesPerPump) return EXIT_FAILURE;

  session.Stop();
  if (session.IsRunning() || runtime.stop_count != 1) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
