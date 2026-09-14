#pragma once

#include <cstddef>
#include <span>

#include "Defs.h"
#include "Renderer.h"
#include "Platform/IEmulatorRuntime.h"

enum class fellow_runtime_error_codes
{
  FELLOW_RUNTIME_ERROR_NO_ERROR = 0,
  FELLOW_RUNTIME_ERROR_CPU_PC_BAD_BANK = 1
};

enum class FELLOW_REQUESTER_TYPE
{
  FELLOW_REQUESTER_TYPE_NONE = 0,
  FELLOW_REQUESTER_TYPE_INFO = 1,
  FELLOW_REQUESTER_TYPE_WARN = 2,
  FELLOW_REQUESTER_TYPE_ERROR = 3
};

extern BOOLE fellow_request_emulation_stop;

extern void fellowRun();
extern void fellowStepOne();
extern void fellowStepOver();
extern void fellowRunDebug(uint32_t breakpoint);
extern void fellowSetRuntimeErrorCode(fellow_runtime_error_codes error_code);
extern void fellowNastyExit();
extern char *fellowGetVersionString();
extern void fellowSetPreStartReset(bool reset);
extern bool fellowGetPreStartReset();
extern BOOLE fellowSaveState(char *filename);
extern BOOLE fellowLoadState(char *filename);
extern void fellowSoftReset();
extern void fellowHardReset();
extern bool fellowEmulationStart();
extern void fellowEmulationStop();
extern void fellowRequestEmulationStop();

extern void fellowShowRequester(FELLOW_REQUESTER_TYPE, const char *, ...);

namespace FellowNG::Runtime
{
  class WinFellowRuntime final : public Platform::IEmulatorRuntime
  {
  public:
    using ModulesStartup = void (*)(int argc, const char **argv);
    using ModulesShutdown = void (*)();

    WinFellowRuntime(
        int argc = 0,
        const char **argv = nullptr,
        ModulesStartup modules_startup = nullptr,
        ModulesShutdown modules_shutdown = nullptr)
      : _argc(argc), _argv(argv), _modules_startup(modules_startup), _modules_shutdown(modules_shutdown)
    {
    }

    ~WinFellowRuntime() override
    {
      Stop();
    }

    bool Start(Platform::IVideoOutput &video, Platform::IAudioOutput &audio) override
    {
      if (_running) return true;

      _video = &video;
      _audio = &audio;
      fellowSetPreStartReset(true);

      if (_modules_startup != nullptr)
      {
        _modules_startup(_argc, _argv);
        _modules_started = true;
      }

      drawSetFramePresentCallback(&WinFellowRuntime::PresentFrame, this);

      if (!fellowEmulationStart())
      {
        drawSetFramePresentCallback(nullptr, nullptr);
        if (_modules_started && _modules_shutdown != nullptr) _modules_shutdown();
        _modules_started = false;
        _video = nullptr;
        _audio = nullptr;
        return false;
      }

      _emulation_started = true;
      _running = true;
      return true;
    }

    void Stop() override
    {
      drawSetFramePresentCallback(nullptr, nullptr);

      if (_emulation_started)
      {
        fellowRequestEmulationStop();
        fellowEmulationStop();
        _emulation_started = false;
      }

      if (_modules_started && _modules_shutdown != nullptr)
      {
        _modules_shutdown();
      }

      _modules_started = false;
      _running = false;
      _video = nullptr;
      _audio = nullptr;
    }

    bool IsRunning() const override
    {
      return _running;
    }

    void HandleInput(const Platform::InputEvent &event) override
    {
      if (event.type == Platform::InputType::Quit) fellowRequestEmulationStop();
    }

    bool RunSlice() override
    {
      if (!_running) return false;

      // Conservative bounded execution for the first real runtime adapter.
      // A later M4.5d slice will replace instruction-at-a-time stepping with
      // a practical bounded scheduler slice for interactive emulation.
      fellowStepOne();
      return _running;
    }

  private:
    static void PresentFrame(const draw_buffer_information &buffer, void *context)
    {
      auto *runtime = static_cast<WinFellowRuntime *>(context);
      if (runtime == nullptr || runtime->_video == nullptr || buffer.top_ptr == nullptr)
      {
        return;
      }

      // The portable video contract currently exposes XRGB8888. Keep the
      // bridge strict until explicit converters for legacy 16/24-bit modes
      // are added.
      if (buffer.bits != 32 || buffer.redsize != 8 || buffer.greensize != 8 || buffer.bluesize != 8 ||
          buffer.redpos != 16 || buffer.greenpos != 8 || buffer.bluepos != 0 ||
          buffer.width == 0 || buffer.height == 0 || buffer.pitch < buffer.width * 4u)
      {
        return;
      }

      const std::size_t byte_count = static_cast<std::size_t>(buffer.pitch) * buffer.height;
      Platform::VideoFrame frame{
        .width = buffer.width,
        .height = buffer.height,
        .pitch_bytes = buffer.pitch,
        .format = Platform::PixelFormat::Xrgb8888,
        .pixels = std::span<const std::byte>(reinterpret_cast<const std::byte *>(buffer.top_ptr), byte_count),
      };

      runtime->_video->Present(frame);
    }

    int _argc = 0;
    const char **_argv = nullptr;
    ModulesStartup _modules_startup = nullptr;
    ModulesShutdown _modules_shutdown = nullptr;
    Platform::IVideoOutput *_video = nullptr;
    Platform::IAudioOutput *_audio = nullptr;
    bool _modules_started = false;
    bool _emulation_started = false;
    bool _running = false;
  };
}
