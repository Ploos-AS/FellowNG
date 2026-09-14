#include "FrontendSession.h"

namespace FellowNG::Platform
{
  namespace
  {
    constexpr unsigned RuntimeSlicesPerPump = 4096;
  }

  FrontendSession::FrontendSession(IEmulatorRuntime &runtime, IInputSource &input, IVideoOutput &video, IAudioOutput &audio)
    : _runtime(runtime), _input(input), _video(video), _audio(audio)
  {
  }

  bool FrontendSession::Start()
  {
    if (_runtime.IsRunning()) return true;
    return _runtime.Start(_video, _audio);
  }

  bool FrontendSession::PumpOnce()
  {
    if (!_runtime.IsRunning()) return false;

    while (const auto event = _input.Poll())
    {
      if (event->type == InputType::Quit)
      {
        Stop();
        return false;
      }
      _runtime.HandleInput(*event);
    }

    // Keep the frontend responsive while giving runtimes whose RunSlice()
    // implementation is intentionally conservative enough work per pump to
    // make forward progress. WinFellowRuntime currently advances one 68k
    // instruction per slice, so a bounded batch avoids an instruction-at-a-time
    // SDL event loop without handing control to the legacy blocking busRun().
    for (unsigned slice = 0; slice < RuntimeSlicesPerPump; ++slice)
    {
      if (!_runtime.RunSlice())
      {
        Stop();
        return false;
      }

      if (!_runtime.IsRunning()) return false;
    }

    return true;
  }

  void FrontendSession::Stop()
  {
    if (_runtime.IsRunning()) _runtime.Stop();
  }

  bool FrontendSession::IsRunning() const
  {
    return _runtime.IsRunning();
  }
}
