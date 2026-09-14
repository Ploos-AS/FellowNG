#include <cassert>

#include "Platform/CallbackEmulatorModuleLifecycle.h"

namespace
{
  int startup_calls = 0;
  int shutdown_calls = 0;
  bool startup_result = true;

  bool Startup(int argc, const char **argv)
  {
    ++startup_calls;
    assert(argc == 2);
    assert(argv != nullptr);
    assert(argv[0] != nullptr);
    assert(argv[1] != nullptr);
    return startup_result;
  }

  void Shutdown()
  {
    ++shutdown_calls;
  }
}

int main()
{
  using FellowNG::Platform::CallbackEmulatorModuleLifecycle;

  const char *argv[] = {"fellowng", "--portable"};
  CallbackEmulatorModuleLifecycle lifecycle(&Startup, &Shutdown);

  assert(!lifecycle.IsStarted());
  assert(lifecycle.Start(2, argv));
  assert(lifecycle.IsStarted());
  assert(startup_calls == 1);

  assert(lifecycle.Start(2, argv));
  assert(startup_calls == 1);

  lifecycle.Stop();
  assert(!lifecycle.IsStarted());
  assert(shutdown_calls == 1);

  lifecycle.Stop();
  assert(shutdown_calls == 1);

  startup_result = false;
  assert(!lifecycle.Start(2, argv));
  assert(!lifecycle.IsStarted());
  assert(startup_calls == 2);
  assert(shutdown_calls == 1);

  return 0;
}
