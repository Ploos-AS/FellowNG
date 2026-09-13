#include "Platform/StdThread.h"

#include <utility>

namespace FellowNG::Platform
{
  StdThread::~StdThread()
  {
    Join();
  }

  bool StdThread::Start(Task task)
  {
    if (_thread.joinable() || !task)
    {
      return false;
    }

    _thread = std::thread(std::move(task));
    return true;
  }

  bool StdThread::Joinable() const
  {
    return _thread.joinable();
  }

  void StdThread::Join()
  {
    if (_thread.joinable())
    {
      _thread.join();
    }
  }
}
