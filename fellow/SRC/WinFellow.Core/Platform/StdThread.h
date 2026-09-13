#pragma once

#include <thread>

#include "Platform/IThread.h"

namespace FellowNG::Platform
{
  class StdThread final : public IThread
  {
  public:
    StdThread() = default;
    ~StdThread() override;

    bool Start(Task task) override;
    bool Joinable() const override;
    void Join() override;

  private:
    std::thread _thread;
  };
}
