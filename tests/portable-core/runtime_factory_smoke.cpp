#include <cstdlib>
#include <memory>

#include "Platform/CallbackEmulatorRuntimeFactory.h"

namespace
{
  class TestRuntime final : public FellowNG::Platform::IEmulatorRuntime
  {
  public:
    bool Start(FellowNG::Platform::IVideoOutput &, FellowNG::Platform::IAudioOutput &) override { return true; }
    void Stop() override {}
    bool IsRunning() const override { return true; }
    void HandleInput(const FellowNG::Platform::InputEvent &) override {}
    bool RunSlice() override { return true; }
  };

  std::unique_ptr<FellowNG::Platform::IEmulatorRuntime> CreateTestRuntime()
  {
    return std::make_unique<TestRuntime>();
  }
}

int main()
{
  FellowNG::Platform::CallbackEmulatorRuntimeFactory factory(&CreateTestRuntime);
  auto runtime = factory.Create();
  if (!runtime) return EXIT_FAILURE;
  if (!runtime->IsRunning()) return EXIT_FAILURE;

  FellowNG::Platform::CallbackEmulatorRuntimeFactory empty_factory(nullptr);
  if (empty_factory.Create() != nullptr) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
