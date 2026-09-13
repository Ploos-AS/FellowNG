#include "Platform/ManualResetEvent.h"

namespace FellowNG::Platform
{
  ManualResetEvent::ManualResetEvent(bool signaled) : _signaled(signaled)
  {
  }

  void ManualResetEvent::Set()
  {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _signaled = true;
    }
    _condition.notify_all();
  }

  void ManualResetEvent::Reset()
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _signaled = false;
  }

  void ManualResetEvent::Wait()
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this] { return _signaled; });
  }

  bool ManualResetEvent::WaitFor(std::chrono::milliseconds timeout)
  {
    std::unique_lock<std::mutex> lock(_mutex);
    return _condition.wait_for(lock, timeout, [this] { return _signaled; });
  }
}
