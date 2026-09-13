#pragma once

#include <condition_variable>
#include <mutex>

#include "Platform/IEvent.h"

namespace FellowNG::Platform
{
  class ManualResetEvent final : public IEvent
  {
  public:
    explicit ManualResetEvent(bool signaled = false);

    void Set() override;
    void Reset() override;
    void Wait() override;
    bool WaitFor(std::chrono::milliseconds timeout) override;

  private:
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _signaled;
  };
}
