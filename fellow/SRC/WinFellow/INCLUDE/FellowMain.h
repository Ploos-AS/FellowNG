#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "Defs.h"
#include "Gameports.h"
#include "Keyboard.h"
#include "Keycode.h"
#include "Renderer.h"
#include "Platform/IEmulatorRuntime.h"
#include "VirtualHost/CoreFactory.h"
#include "VirtualHost/PortableAudioPlatformFactory.h"

enum class fellow_runtime_error_codes
{
  FELLOW_RUNTIME_ERROR_NO_ERROR = 0,
  FELLOW_RUNTIME_ERROR_CPU_PC_BAD_BANK = 1
};

enum class FELLOW_REQUESTER_TYPE
{
  FELLOW_REQUESTER_TYPE_NONE = 0,
  FELLOW_REQUESTER_TYPE_INFO = 1,
  FELLOW_REQUESTER_TYPE_WARN = 2,
  FELLOW_REQUESTER_TYPE_ERROR = 3
};

extern BOOLE fellow_request_emulation_stop;

extern void fellowRun();
extern void fellowStepOne();
extern void fellowStepOver();
extern void fellowRunDebug(uint32_t breakpoint);
extern void fellowSetRuntimeErrorCode(fellow_runtime_error_codes error_code);
extern void fellowNastyExit();
extern char *fellowGetVersionString();
extern void fellowSetPreStartReset(bool reset);
extern bool fellowGetPreStartReset();
extern BOOLE fellowSaveState(char *filename);
extern BOOLE fellowLoadState(char *filename);
extern void fellowSoftReset();
extern void fellowHardReset();
extern bool fellowEmulationStart();
extern void fellowEmulationStop();
extern void fellowRequestEmulationStop();

extern void fellowShowRequester(FELLOW_REQUESTER_TYPE, const char *, ...);

namespace FellowNG::Runtime
{
  class WinFellowRuntime final : public Platform::IEmulatorRuntime
  {
  public:
    using ModulesStartup = void (*)(int argc, const char **argv);
    using ModulesShutdown = void (*)();

    WinFellowRuntime(
        int argc = 0,
        const char **argv = nullptr,
        ModulesStartup modules_startup = nullptr,
        ModulesShutdown modules_shutdown = nullptr)
      : _argc(argc), _argv(argv), _modules_startup(modules_startup), _modules_shutdown(modules_shutdown)
    {
    }

    ~WinFellowRuntime() override
    {
      Stop();
    }

    bool Start(Platform::IVideoOutput &video, Platform::IAudioOutput &audio) override
    {
      if (_running) return true;

      _video = &video;
      _audio = &audio;
      fellowSetPreStartReset(true);

      _previous_platform_factory = CoreFactory::GetPlatformFactory();
      if (_previous_platform_factory == nullptr)
      {
        _video = nullptr;
        _audio = nullptr;
        return false;
      }

      _audio_platform_factory = new PortableAudioPlatformFactory(*_previous_platform_factory, audio);
      CoreFactory::SetPlatformFactory(_audio_platform_factory);

      if (_modules_startup != nullptr)
      {
        _modules_startup(_argc, _argv);
        _modules_started = true;
      }

      drawSetFramePresentCallback(&WinFellowRuntime::PresentFrame, this);

      if (!fellowEmulationStart())
      {
        drawSetFramePresentCallback(nullptr, nullptr);
        if (_modules_started && _modules_shutdown != nullptr) _modules_shutdown();
        _modules_started = false;
        RestoreAudioPlatformFactory();
        _video = nullptr;
        _audio = nullptr;
        return false;
      }

      _emulation_started = true;
      _running = true;
      return true;
    }

    void Stop() override
    {
      drawSetFramePresentCallback(nullptr, nullptr);

      if (_emulation_started)
      {
        fellowRequestEmulationStop();
        fellowEmulationStop();
        _emulation_started = false;
      }

      if (_modules_started && _modules_shutdown != nullptr) _modules_shutdown();

      _modules_started = false;
      RestoreAudioPlatformFactory();
      _running = false;
      _video = nullptr;
      _audio = nullptr;
    }

    bool IsRunning() const override
    {
      return _running;
    }

    void HandleInput(const Platform::InputEvent &event) override
    {
      switch (event.type)
      {
        case Platform::InputType::Quit:
          fellowRequestEmulationStop();
          break;

        case Platform::InputType::Key:
        {
          const uint8_t key = TranslateKey(static_cast<Platform::KeyCode>(event.code));
          if (key != A_NONE) kbdKeyAdd(static_cast<uint8_t>(key | (event.pressed ? 0x00 : 0x80)));
          break;
        }

        case Platform::InputType::MouseMove:
          _mouse_x += event.value;
          _mouse_y += event.value2;
          SubmitMouse();
          break;

        case Platform::InputType::MouseButton:
          if (event.code == static_cast<std::int32_t>(Platform::MouseButton::Left)) _mouse_left = event.pressed;
          else if (event.code == static_cast<std::int32_t>(Platform::MouseButton::Middle)) _mouse_middle = event.pressed;
          else if (event.code == static_cast<std::int32_t>(Platform::MouseButton::Right)) _mouse_right = event.pressed;
          SubmitMouse();
          break;

        case Platform::InputType::JoystickAxis:
          if (event.code == static_cast<std::int32_t>(Platform::JoystickAxis::LeftX)) _joy_x = event.value;
          else if (event.code == static_cast<std::int32_t>(Platform::JoystickAxis::LeftY)) _joy_y = event.value;
          SubmitJoystick();
          break;

        case Platform::InputType::JoystickButton:
          if (event.code == static_cast<std::int32_t>(Platform::JoystickButton::South)) _joy_fire0 = event.pressed;
          else if (event.code == static_cast<std::int32_t>(Platform::JoystickButton::East)) _joy_fire1 = event.pressed;
          SubmitJoystick();
          break;
      }
    }

    bool RunSlice() override
    {
      if (!_running) return false;
      fellowStepOne();
      return _running;
    }

  private:
    void RestoreAudioPlatformFactory()
    {
      if (_audio_platform_factory == nullptr) return;

      CoreFactory::SetPlatformFactory(_previous_platform_factory);
      delete _audio_platform_factory;
      _audio_platform_factory = nullptr;
      _previous_platform_factory = nullptr;
    }

    static uint8_t TranslateKey(Platform::KeyCode key)
    {
      using K = Platform::KeyCode;
      switch (key)
      {
        case K::Escape: return A_ESCAPE;
        case K::F1: return A_F1; case K::F2: return A_F2; case K::F3: return A_F3; case K::F4: return A_F4; case K::F5: return A_F5;
        case K::F6: return A_F6; case K::F7: return A_F7; case K::F8: return A_F8; case K::F9: return A_F9; case K::F10: return A_F10;
        case K::Grave: return A_GRAVE;
        case K::Digit1: return A_1; case K::Digit2: return A_2; case K::Digit3: return A_3; case K::Digit4: return A_4; case K::Digit5: return A_5;
        case K::Digit6: return A_6; case K::Digit7: return A_7; case K::Digit8: return A_8; case K::Digit9: return A_9; case K::Digit0: return A_0;
        case K::Minus: return A_MINUS; case K::Equals: return A_EQUALS; case K::Backspace: return A_BACKSPACE; case K::Tab: return A_TAB;
        case K::Q: return A_Q; case K::W: return A_W; case K::E: return A_E; case K::R: return A_R; case K::T: return A_T;
        case K::Y: return A_Y; case K::U: return A_U; case K::I: return A_I; case K::O: return A_O; case K::P: return A_P;
        case K::LeftBracket: return A_LEFT_BRACKET; case K::RightBracket: return A_RIGHT_BRACKET; case K::Return: return A_RETURN;
        case K::LeftCtrl: return A_CTRL; case K::CapsLock: return A_CAPS_LOCK;
        case K::A: return A_A; case K::S: return A_S; case K::D: return A_D; case K::F: return A_F; case K::G: return A_G;
        case K::H: return A_H; case K::J: return A_J; case K::K: return A_K; case K::L: return A_L; case K::Semicolon: return A_SEMICOLON;
        case K::Apostrophe: return A_APOSTROPHE; case K::Backslash: return A_BACKSLASH;
        case K::LeftShift: return A_LEFT_SHIFT; case K::NonUsBackslash: return A_LESS_THAN;
        case K::Z: return A_Z; case K::X: return A_X; case K::C: return A_C; case K::V: return A_V; case K::B: return A_B;
        case K::N: return A_N; case K::M: return A_M; case K::Comma: return A_COMMA; case K::Period: return A_PERIOD; case K::Slash: return A_SLASH;
        case K::RightShift: return A_RIGHT_SHIFT; case K::LeftAlt: return A_LEFT_ALT; case K::LeftMeta: return A_LEFT_AMIGA;
        case K::Space: return A_SPACE; case K::RightMeta: return A_RIGHT_AMIGA; case K::RightAlt: return A_RIGHT_ALT;
        case K::Delete: return A_DELETE; case K::Up: return A_UP; case K::Left: return A_LEFT; case K::Down: return A_DOWN; case K::Right: return A_RIGHT;
        case K::NumpadDivide: return A_NUMPAD_DIVIDE; case K::NumpadMultiply: return A_NUMPAD_MULTIPLY; case K::NumpadMinus: return A_NUMPAD_MINUS;
        case K::Numpad7: return A_NUMPAD_7; case K::Numpad8: return A_NUMPAD_8; case K::Numpad9: return A_NUMPAD_9;
        case K::Numpad4: return A_NUMPAD_4; case K::Numpad5: return A_NUMPAD_5; case K::Numpad6: return A_NUMPAD_6; case K::NumpadPlus: return A_NUMPAD_PLUS;
        case K::Numpad1: return A_NUMPAD_1; case K::Numpad2: return A_NUMPAD_2; case K::Numpad3: return A_NUMPAD_3; case K::NumpadEnter: return A_NUMPAD_ENTER;
        case K::Numpad0: return A_NUMPAD_0; case K::NumpadPeriod: return A_NUMPAD_DOT;
        default: return A_NONE;
      }
    }

    void SubmitMouse()
    {
      for (uint32_t port = 0; port < 2; ++port)
      {
        if (gameport_input[port] == GP_MOUSE0 || gameport_input[port] == GP_MOUSE1)
        {
          const auto device = gameport_input[port];
          gameportMouseHandler(device, _mouse_x, _mouse_y, _mouse_left, _mouse_middle, _mouse_right);
          _mouse_x = 0;
          _mouse_y = 0;
          return;
        }
      }
    }

    void SubmitJoystick()
    {
      constexpr int deadzone = 8192;
      const BOOLE left = _joy_x < -deadzone;
      const BOOLE right = _joy_x > deadzone;
      const BOOLE up = _joy_y < -deadzone;
      const BOOLE down = _joy_y > deadzone;

      for (uint32_t port = 0; port < 2; ++port)
      {
        const auto device = gameport_input[port];
        if (device != GP_NONE && device != GP_MOUSE0 && device != GP_MOUSE1)
        {
          gameportJoystickHandler(device, left, up, right, down, _joy_fire0, _joy_fire1);
          return;
        }
      }
    }

    static void PresentFrame(const draw_buffer_information &buffer, void *context)
    {
      auto *runtime = static_cast<WinFellowRuntime *>(context);
      if (runtime == nullptr || runtime->_video == nullptr || buffer.top_ptr == nullptr) return;

      if (buffer.bits != 32 || buffer.redsize != 8 || buffer.greensize != 8 || buffer.bluesize != 8 ||
          buffer.redpos != 16 || buffer.greenpos != 8 || buffer.bluepos != 0 ||
          buffer.width == 0 || buffer.height == 0 || buffer.pitch < buffer.width * 4u)
      {
        return;
      }

      const std::size_t byte_count = static_cast<std::size_t>(buffer.pitch) * buffer.height;
      Platform::VideoFrame frame{
        .width = buffer.width,
        .height = buffer.height,
        .pitch_bytes = buffer.pitch,
        .format = Platform::PixelFormat::Xrgb8888,
        .pixels = std::span<const std::byte>(reinterpret_cast<const std::byte *>(buffer.top_ptr), byte_count),
      };
      runtime->_video->Present(frame);
    }

    int _argc = 0;
    const char **_argv = nullptr;
    ModulesStartup _modules_startup = nullptr;
    ModulesShutdown _modules_shutdown = nullptr;
    Platform::IVideoOutput *_video = nullptr;
    Platform::IAudioOutput *_audio = nullptr;
    ICorePlatformFactory *_previous_platform_factory = nullptr;
    PortableAudioPlatformFactory *_audio_platform_factory = nullptr;
    std::int32_t _mouse_x = 0;
    std::int32_t _mouse_y = 0;
    bool _mouse_left = false;
    bool _mouse_middle = false;
    bool _mouse_right = false;
    std::int32_t _joy_x = 0;
    std::int32_t _joy_y = 0;
    bool _joy_fire0 = false;
    bool _joy_fire1 = false;
    bool _modules_started = false;
    bool _emulation_started = false;
    bool _running = false;
  };
}
