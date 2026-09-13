#pragma once

#include <functional>

namespace FellowNG::Platform
{
  class IThread
  {
  public:
    using Task = std::function<void()>;

    virtual ~IThread() = default;
    virtual bool Start(Task task) = 0;
    virtual bool Joinable() const = 0;
    virtual void Join() = 0;
  };
}
