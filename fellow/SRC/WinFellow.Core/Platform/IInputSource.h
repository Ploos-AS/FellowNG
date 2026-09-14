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

  enum class KeyCode : std::int32_t
  {
    Unknown = 0,
    Escape,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
    Grave,
    Digit1, Digit2, Digit3, Digit4, Digit5,
    Digit6, Digit7, Digit8, Digit9, Digit0,
    Minus, Equals, Backspace,
    Tab,
    Q, W, E, R, T, Y, U, I, O, P,
    LeftBracket, RightBracket, Return,
    LeftCtrl, CapsLock,
    A, S, D, F, G, H, J, K, L,
    Semicolon, Apostrophe, Backslash,
    LeftShift, NonUsBackslash,
    Z, X, C, V, B, N, M,
    Comma, Period, Slash, RightShift,
    LeftAlt, LeftMeta, Space, RightMeta, RightAlt,
    Delete,
    Up, Left, Down, Right,
    NumpadDivide, NumpadMultiply, NumpadMinus,
    Numpad7, Numpad8, Numpad9, Numpad4, Numpad5, Numpad6,
    NumpadPlus, Numpad1, Numpad2, Numpad3, NumpadEnter,
    Numpad0, NumpadPeriod
  };

  enum class MouseButton : std::int32_t
  {
    Left = 1,
    Middle = 2,
    Right = 3
  };

  enum class JoystickAxis : std::int32_t
  {
    LeftX = 0,
    LeftY = 1
  };

  enum class JoystickButton : std::int32_t
  {
    South = 0,
    East = 1
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
