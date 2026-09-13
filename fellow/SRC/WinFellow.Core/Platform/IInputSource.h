#pragma once

#include <cstdint>
#include <optional>

namespace FellowNG::Platform
{
  enum class InputType
  {
    Key,
    MouseMove,
    MouseButton,
    JoystickAxis,
    JoystickButton,
    Quit
  };

  struct InputEvent
  {
    InputType type = InputType::Key;
    std::int32_t code = 0;
    std::int32_t value = 0;
    std::int32_t value2 = 0;
    bool pressed = false;
  };

  class IInputSource
  {
  public:
    virtual ~IInputSource() = default;
    virtual std::optional<InputEvent> Poll() = 0;
  };
}
