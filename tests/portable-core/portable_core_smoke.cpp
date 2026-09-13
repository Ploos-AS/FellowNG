#include <array>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>

#include "CustomChipset/RegisterUtility.h"
#include "CustomChipset/Registers.h"
#include "Platform/HostLifecycle.h"
#include "Platform/IAudioOutput.h"
#include "Platform/IInputSource.h"
#include "Platform/IVideoOutput.h"
#include "Platform/ManualResetEvent.h"
#include "Platform/StdClock.h"
#include "Platform/StdFileSystem.h"
#include "Platform/StdThread.h"
#include "Platform/StreamLogger.h"

namespace
{
  bool check(bool condition)
  {
    return condition;
  }

  class TestAudioOutput final : public FellowNG::Platform::IAudioOutput
  {
  public:
    bool Start(const FellowNG::Platform::AudioFormat &format) override
    {
      format_ = format;
      running_ = true;
      return true;
    }

    void Stop() override { running_ = false; }
    bool IsRunning() const override { return running_; }

    bool SubmitInterleaved(std::span<const std::int16_t> samples) override
    {
      if (!running_) return false;
      submitted_samples_ += samples.size();
      return true;
    }

    std::size_t submitted_samples() const { return submitted_samples_; }

  private:
    FellowNG::Platform::AudioFormat format_{};
    bool running_ = false;
    std::size_t submitted_samples_ = 0;
  };

  class TestVideoOutput final : public FellowNG::Platform::IVideoOutput
  {
  public:
    bool Start(std::uint32_t width, std::uint32_t height) override
    {
      width_ = width;
      height_ = height;
      running_ = true;
      return true;
    }

    void Stop() override { running_ = false; }
    bool IsRunning() const override { return running_; }

    bool Present(const FellowNG::Platform::VideoFrame &frame) override
    {
      if (!running_ || frame.width != width_ || frame.height != height_) return false;
      ++presented_frames_;
      return true;
    }

    std::size_t presented_frames() const { return presented_frames_; }

  private:
    std::uint32_t width_ = 0;
    std::uint32_t height_ = 0;
    bool running_ = false;
    std::size_t presented_frames_ = 0;
  };

  class TestInputSource final : public FellowNG::Platform::IInputSource
  {
  public:
    std::optional<FellowNG::Platform::InputEvent> Poll() override
    {
      if (consumed_) return std::nullopt;
      consumed_ = true;
      return FellowNG::Platform::InputEvent{
        .type = FellowNG::Platform::InputType::Key,
        .code = 42,
        .value = 0,
        .value2 = 0,
        .pressed = true};
    }

  private:
    bool consumed_ = false;
  };
}

int main()
{
  CustomChipset::Registers registers;
  CustomChipset::RegisterUtility utility(registers);

  if (!check(utility.IsLoresEnabled())) return EXIT_FAILURE;
  if (!check(!utility.IsHiresEnabled())) return EXIT_FAILURE;
  if (!check(utility.GetEnabledBitplaneCount() == 0)) return EXIT_FAILURE;
  if (!check(!utility.IsMasterDMAEnabled())) return EXIT_FAILURE;

  registers.BplCon0 = static_cast<uint16_t>(0x8000 | 0x4000 | 0x0800 | 0x0400 | 0x0004);
  registers.BplCon2 = 0x0040;
  registers.DmaConR = static_cast<uint16_t>(0x0200 | 0x0100 | 0x0010 | 0x0400);

  if (!check(utility.IsHiresEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsDualPlayfieldEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsHAMEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsInterlaceEnabled())) return EXIT_FAILURE;
  if (!check(utility.GetEnabledBitplaneCount() == 4)) return EXIT_FAILURE;
  if (!check(utility.IsPlayfield2PriorityEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsMasterDMAEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsMasterDMAAndBitplaneDMAEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsDiskDMAEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsBlitterPriorityEnabled())) return EXIT_FAILURE;

  FellowNG::Platform::StdClock clock;
  const auto before = clock.Now();
  clock.SleepFor(std::chrono::milliseconds(1));
  const auto after = clock.Now();
  if (!check(after >= before)) return EXIT_FAILURE;

  FellowNG::Platform::StdFileSystem file_system;
  const auto current = file_system.Stat(std::filesystem::current_path());
  if (!check(current.exists && current.directory)) return EXIT_FAILURE;
  if (!check(file_system.Absolute(".").is_absolute())) return EXIT_FAILURE;

  std::ostringstream log_output;
  FellowNG::Platform::StreamLogger logger(log_output);
  FellowNG::Platform::HostLifecycle host(logger);

  if (!check(!host.IsRunning())) return EXIT_FAILURE;
  if (!check(host.Start())) return EXIT_FAILURE;
  if (!check(host.IsRunning())) return EXIT_FAILURE;
  if (!check(host.Start())) return EXIT_FAILURE;
  host.Stop();
  if (!check(!host.IsRunning())) return EXIT_FAILURE;
  host.Stop();

  const std::string log = log_output.str();
  if (!check(log.find("[INFO] host starting") != std::string::npos)) return EXIT_FAILURE;
  if (!check(log.find("[INFO] host stopped") != std::string::npos)) return EXIT_FAILURE;
  if (!check(log.find("[DEBUG] host already running") != std::string::npos)) return EXIT_FAILURE;

  FellowNG::Platform::ManualResetEvent event;
  if (!check(!event.WaitFor(std::chrono::milliseconds(1)))) return EXIT_FAILURE;

  std::atomic<bool> worker_released{false};
  FellowNG::Platform::StdThread worker;
  if (!check(worker.Start([&event, &worker_released] {
        event.Wait();
        worker_released.store(true);
      }))) return EXIT_FAILURE;
  if (!check(worker.Joinable())) return EXIT_FAILURE;
  if (!check(!worker.Start([] {}))) return EXIT_FAILURE;

  event.Set();
  worker.Join();
  if (!check(!worker.Joinable())) return EXIT_FAILURE;
  if (!check(worker_released.load())) return EXIT_FAILURE;
  if (!check(event.WaitFor(std::chrono::milliseconds(0)))) return EXIT_FAILURE;

  event.Reset();
  if (!check(!event.WaitFor(std::chrono::milliseconds(1)))) return EXIT_FAILURE;

  TestAudioOutput audio;
  if (!check(audio.Start({.sample_rate = 44100, .channels = 2}))) return EXIT_FAILURE;
  const std::array<std::int16_t, 4> audio_samples{1, -1, 2, -2};
  if (!check(audio.SubmitInterleaved(audio_samples))) return EXIT_FAILURE;
  if (!check(audio.submitted_samples() == audio_samples.size())) return EXIT_FAILURE;
  audio.Stop();
  if (!check(!audio.IsRunning())) return EXIT_FAILURE;

  TestVideoOutput video;
  if (!check(video.Start(2, 2))) return EXIT_FAILURE;
  const std::array<std::byte, 16> pixels{};
  const FellowNG::Platform::VideoFrame frame{
    .width = 2,
    .height = 2,
    .pitch_bytes = 8,
    .format = FellowNG::Platform::PixelFormat::Xrgb8888,
    .pixels = pixels};
  if (!check(video.Present(frame))) return EXIT_FAILURE;
  if (!check(video.presented_frames() == 1)) return EXIT_FAILURE;
  video.Stop();

  TestInputSource input;
  const auto input_event = input.Poll();
  if (!check(input_event.has_value())) return EXIT_FAILURE;
  if (!check(input_event->type == FellowNG::Platform::InputType::Key)) return EXIT_FAILURE;
  if (!check(input_event->code == 42 && input_event->pressed)) return EXIT_FAILURE;
  if (!check(!input.Poll().has_value())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
