#pragma once

#include <optional>

#include <SDL3/SDL.h>

#include "Platform/IInputSource.h"

namespace FellowNG::Frontend::SDL
{
  class SdlInputSource final : public Platform::IInputSource
  {
  public:
    std::optional<Platform::InputEvent> Poll() override;

  private:
    static std::optional<Platform::InputEvent> Translate(const SDL_Event &event);
  };
}
