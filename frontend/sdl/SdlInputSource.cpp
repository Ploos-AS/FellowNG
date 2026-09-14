#include "SdlInputSource.h"

namespace FellowNG::Frontend::SDL
{
  namespace
  {
    Platform::KeyCode TranslateKey(SDL_Scancode scancode)
    {
      using K = Platform::KeyCode;
      switch (scancode)
      {
        case SDL_SCANCODE_ESCAPE: return K::Escape;
        case SDL_SCANCODE_F1: return K::F1; case SDL_SCANCODE_F2: return K::F2;
        case SDL_SCANCODE_F3: return K::F3; case SDL_SCANCODE_F4: return K::F4;
        case SDL_SCANCODE_F5: return K::F5; case SDL_SCANCODE_F6: return K::F6;
        case SDL_SCANCODE_F7: return K::F7; case SDL_SCANCODE_F8: return K::F8;
        case SDL_SCANCODE_F9: return K::F9; case SDL_SCANCODE_F10: return K::F10;
        case SDL_SCANCODE_GRAVE: return K::Grave;
        case SDL_SCANCODE_1: return K::Digit1; case SDL_SCANCODE_2: return K::Digit2;
        case SDL_SCANCODE_3: return K::Digit3; case SDL_SCANCODE_4: return K::Digit4;
        case SDL_SCANCODE_5: return K::Digit5; case SDL_SCANCODE_6: return K::Digit6;
        case SDL_SCANCODE_7: return K::Digit7; case SDL_SCANCODE_8: return K::Digit8;
        case SDL_SCANCODE_9: return K::Digit9; case SDL_SCANCODE_0: return K::Digit0;
        case SDL_SCANCODE_MINUS: return K::Minus; case SDL_SCANCODE_EQUALS: return K::Equals;
        case SDL_SCANCODE_BACKSPACE: return K::Backspace; case SDL_SCANCODE_TAB: return K::Tab;
        case SDL_SCANCODE_Q: return K::Q; case SDL_SCANCODE_W: return K::W;
        case SDL_SCANCODE_E: return K::E; case SDL_SCANCODE_R: return K::R;
        case SDL_SCANCODE_T: return K::T; case SDL_SCANCODE_Y: return K::Y;
        case SDL_SCANCODE_U: return K::U; case SDL_SCANCODE_I: return K::I;
        case SDL_SCANCODE_O: return K::O; case SDL_SCANCODE_P: return K::P;
        case SDL_SCANCODE_LEFTBRACKET: return K::LeftBracket; case SDL_SCANCODE_RIGHTBRACKET: return K::RightBracket;
        case SDL_SCANCODE_RETURN: return K::Return;
        case SDL_SCANCODE_LCTRL: return K::LeftCtrl; case SDL_SCANCODE_CAPSLOCK: return K::CapsLock;
        case SDL_SCANCODE_A: return K::A; case SDL_SCANCODE_S: return K::S;
        case SDL_SCANCODE_D: return K::D; case SDL_SCANCODE_F: return K::F;
        case SDL_SCANCODE_G: return K::G; case SDL_SCANCODE_H: return K::H;
        case SDL_SCANCODE_J: return K::J; case SDL_SCANCODE_K: return K::K;
        case SDL_SCANCODE_L: return K::L; case SDL_SCANCODE_SEMICOLON: return K::Semicolon;
        case SDL_SCANCODE_APOSTROPHE: return K::Apostrophe; case SDL_SCANCODE_BACKSLASH: return K::Backslash;
        case SDL_SCANCODE_LSHIFT: return K::LeftShift; case SDL_SCANCODE_NONUSBACKSLASH: return K::NonUsBackslash;
        case SDL_SCANCODE_Z: return K::Z; case SDL_SCANCODE_X: return K::X;
        case SDL_SCANCODE_C: return K::C; case SDL_SCANCODE_V: return K::V;
        case SDL_SCANCODE_B: return K::B; case SDL_SCANCODE_N: return K::N;
        case SDL_SCANCODE_M: return K::M; case SDL_SCANCODE_COMMA: return K::Comma;
        case SDL_SCANCODE_PERIOD: return K::Period; case SDL_SCANCODE_SLASH: return K::Slash;
        case SDL_SCANCODE_RSHIFT: return K::RightShift;
        case SDL_SCANCODE_LALT: return K::LeftAlt; case SDL_SCANCODE_LGUI: return K::LeftMeta;
        case SDL_SCANCODE_SPACE: return K::Space; case SDL_SCANCODE_RGUI: return K::RightMeta;
        case SDL_SCANCODE_RALT: return K::RightAlt; case SDL_SCANCODE_DELETE: return K::Delete;
        case SDL_SCANCODE_UP: return K::Up; case SDL_SCANCODE_LEFT: return K::Left;
        case SDL_SCANCODE_DOWN: return K::Down; case SDL_SCANCODE_RIGHT: return K::Right;
        case SDL_SCANCODE_KP_DIVIDE: return K::NumpadDivide; case SDL_SCANCODE_KP_MULTIPLY: return K::NumpadMultiply;
        case SDL_SCANCODE_KP_MINUS: return K::NumpadMinus; case SDL_SCANCODE_KP_7: return K::Numpad7;
        case SDL_SCANCODE_KP_8: return K::Numpad8; case SDL_SCANCODE_KP_9: return K::Numpad9;
        case SDL_SCANCODE_KP_4: return K::Numpad4; case SDL_SCANCODE_KP_5: return K::Numpad5;
        case SDL_SCANCODE_KP_6: return K::Numpad6; case SDL_SCANCODE_KP_PLUS: return K::NumpadPlus;
        case SDL_SCANCODE_KP_1: return K::Numpad1; case SDL_SCANCODE_KP_2: return K::Numpad2;
        case SDL_SCANCODE_KP_3: return K::Numpad3; case SDL_SCANCODE_KP_ENTER: return K::NumpadEnter;
        case SDL_SCANCODE_KP_0: return K::Numpad0; case SDL_SCANCODE_KP_PERIOD: return K::NumpadPeriod;
        default: return K::Unknown;
      }
    }
  }

  std::optional<Platform::InputEvent> SdlInputSource::Poll()
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (auto translated = Translate(event)) return translated;
    }
    return std::nullopt;
  }

  std::optional<Platform::InputEvent> SdlInputSource::Translate(const SDL_Event &event)
  {
    Platform::InputEvent result{};
    switch (event.type)
    {
      case SDL_EVENT_QUIT:
        result.type = Platform::InputType::Quit;
        return result;
      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP:
      {
        const auto key = TranslateKey(event.key.scancode);
        if (key == Platform::KeyCode::Unknown) return std::nullopt;
        result.type = Platform::InputType::Key;
        result.code = static_cast<std::int32_t>(key);
        result.pressed = event.type == SDL_EVENT_KEY_DOWN;
        return result;
      }
      case SDL_EVENT_MOUSE_MOTION:
        result.type = Platform::InputType::MouseMove;
        result.value = static_cast<std::int32_t>(event.motion.xrel);
        result.value2 = static_cast<std::int32_t>(event.motion.yrel);
        return result;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
        result.type = Platform::InputType::MouseButton;
        if (event.button.button == SDL_BUTTON_LEFT) result.code = static_cast<std::int32_t>(Platform::MouseButton::Left);
        else if (event.button.button == SDL_BUTTON_MIDDLE) result.code = static_cast<std::int32_t>(Platform::MouseButton::Middle);
        else if (event.button.button == SDL_BUTTON_RIGHT) result.code = static_cast<std::int32_t>(Platform::MouseButton::Right);
        else return std::nullopt;
        result.pressed = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
        return result;
      case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        if (event.gaxis.axis != SDL_GAMEPAD_AXIS_LEFTX && event.gaxis.axis != SDL_GAMEPAD_AXIS_LEFTY) return std::nullopt;
        result.type = Platform::InputType::JoystickAxis;
        result.code = event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX
            ? static_cast<std::int32_t>(Platform::JoystickAxis::LeftX)
            : static_cast<std::int32_t>(Platform::JoystickAxis::LeftY);
        result.value = static_cast<std::int32_t>(event.gaxis.value);
        return result;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
      case SDL_EVENT_GAMEPAD_BUTTON_UP:
        if (event.gbutton.button != SDL_GAMEPAD_BUTTON_SOUTH && event.gbutton.button != SDL_GAMEPAD_BUTTON_EAST) return std::nullopt;
        result.type = Platform::InputType::JoystickButton;
        result.code = event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH
            ? static_cast<std::int32_t>(Platform::JoystickButton::South)
            : static_cast<std::int32_t>(Platform::JoystickButton::East);
        result.pressed = event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;
        return result;
      default:
        return std::nullopt;
    }
  }
}
