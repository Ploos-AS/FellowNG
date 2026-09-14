#include <cstdlib>
#include <memory>
#include <optional>
#include <span>

#include "Platform/CallbackEmulatorRuntimeFactory.h"
#include "SdlFrontendSession.h"

namespace
{
  class NullInput final : public FellowNG::Platform::IInputSource
  {
  public:
    std::optional<FellowNG::Platform::InputEvent> Poll() override { return std::nullopt; }
  };

  class NullVideo final : public FellowNG::Platform::IVideoOutput
  {
  public:
    bool Start(std::uint32_t, std::uint32_t) override { return true; }
    void Stop() override {}
    bool IsRunning() const override { return true; }
    bool Present(const FellowNG::Platform::VideoFrame &) override { return true; }
  };

  class NullAudio final : public FellowNG::Platform::IAudioOutput
  {
  public:
    bool Start(const FellowNG::Platform::AudioFormat &) override { return true; }
    void Stop() override {}
    bool IsRunning() const override { return true; }
    bool SubmitInterleaved(std::span<const std::int16_t>) override { return true; }
  };

  class SmokeRuntime final : public FellowNG::Platform::IEmulatorRuntime
  {
  public:
    bool Start(FellowNG::Platform::IVideoOutput &, FellowNG::Platform::IAudioOutput &) override
    {
      _running = true;
      return true;
    }

    void Stop() override { _running = false; }
    bool IsRunning() const override { return _running; }
    void HandleInput(const FellowNG::Platform::InputEvent &) override {}
    bool RunSlice() override { return _running; }

  private:
    bool _running = false;
  };

  std::unique_ptr<FellowNG::Platform::IEmulatorRuntime> CreateRuntime()
  {
    return std::make_unique<SmokeRuntime>();
  }
}

int main()
{
  NullInput input;
  NullVideo video;
  NullAudio audio;

  FellowNG::Platform::CallbackEmulatorRuntimeFactory factory(&CreateRuntime);
  FellowNG::Frontend::SDL::SdlFrontendSession session(factory, input, video, audio);
  if (!session.HasRuntime()) return EXIT_FAILURE;
  if (!session.Start() || !session.IsRunning()) return EXIT_FAILURE;
  if (!session.PumpOnce()) return EXIT_FAILURE;
  session.Stop();
  if (session.IsRunning()) return EXIT_FAILURE;

  FellowNG::Platform::CallbackEmulatorRuntimeFactory null_factory(nullptr);
  FellowNG::Frontend::SDL::SdlFrontendSession empty_session(null_factory, input, video, audio);
  if (empty_session.HasRuntime()) return EXIT_FAILURE;
  if (empty_session.Start()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
