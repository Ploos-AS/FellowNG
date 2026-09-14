#pragma once

#include "Defs.h"
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
extern void fellowModulesStartup(int argc, const char **argv);
extern void fellowModulesShutdown();

extern void fellowShowRequester(FELLOW_REQUESTER_TYPE, const char *, ...);

namespace FellowNG::Runtime
{
  class WinFellowRuntime final : public Platform::IEmulatorRuntime
  {
  public:
    WinFellowRuntime(int argc = 0, const char **argv = nullptr) : _argc(argc), _argv(argv) {}

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
      fellowModulesStartup(_argc, _argv);
      _modules_started = true;

      if (!fellowEmulationStart())
      {
        fellowModulesShutdown();
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
      if (_emulation_started)
      {
        fellowRequestEmulationStop();
        fellowEmulationStop();
        _emulation_started = false;
      }

      if (_modules_started)
      {
        fellowModulesShutdown();
        _modules_started = false;
      }

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

      // One instruction is deliberately conservative for the first portable
      // runtime adapter. M4.5d can replace this with a larger bounded bus slice
      // once SDL input/video/audio are attached to the real Fellow devices.
      fellowStepOne();
      return _running;
    }

  private:
    int _argc = 0;
    const char **_argv = nullptr;
    Platform::IVideoOutput *_video = nullptr;
    Platform::IAudioOutput *_audio = nullptr;
    bool _modules_started = false;
    bool _emulation_started = false;
    bool _running = false;
  };
}
