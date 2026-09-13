#include "FrontendSession.h"

namespace FellowNG::Platform
{
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

    if (!_runtime.RunSlice())
    {
      Stop();
      return false;
    }

    return _runtime.IsRunning();
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
