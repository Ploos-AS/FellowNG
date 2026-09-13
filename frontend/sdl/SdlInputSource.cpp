#include "SdlInputSource.h"

namespace FellowNG::Frontend::SDL
{
  std::optional<Platform::InputEvent> SdlInputSource::Poll()
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (auto translated = Translate(event))
      {
        return translated;
      }
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
        result.type = Platform::InputType::Key;
        result.code = static_cast<std::int32_t>(event.key.scancode);
        result.pressed = event.type == SDL_EVENT_KEY_DOWN;
        result.value = static_cast<std::int32_t>(event.key.key);
        return result;

      case SDL_EVENT_MOUSE_MOTION:
        result.type = Platform::InputType::MouseMove;
        result.value = static_cast<std::int32_t>(event.motion.xrel);
        result.value2 = static_cast<std::int32_t>(event.motion.yrel);
        return result;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
        result.type = Platform::InputType::MouseButton;
        result.code = static_cast<std::int32_t>(event.button.button);
        result.pressed = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
        return result;

      case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        result.type = Platform::InputType::JoystickAxis;
        result.code = static_cast<std::int32_t>(event.gaxis.axis);
        result.value = static_cast<std::int32_t>(event.gaxis.value);
        result.value2 = static_cast<std::int32_t>(event.gaxis.which);
        return result;

      case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
      case SDL_EVENT_GAMEPAD_BUTTON_UP:
        result.type = Platform::InputType::JoystickButton;
        result.code = static_cast<std::int32_t>(event.gbutton.button);
        result.value = static_cast<std::int32_t>(event.gbutton.which);
        result.pressed = event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;
        return result;

      default:
        return std::nullopt;
    }
  }
}
