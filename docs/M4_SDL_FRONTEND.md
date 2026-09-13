# M4 — SDL3 frontend

M4 turns the portable host contracts from M3 into a native cross-platform frontend. It is delivered incrementally so the SDL layer can be qualified independently before Fellow emulation is attached to it.

## M4.1 — SDL3 bootstrap ✅

Implemented:

- optional `FELLOWNG_BUILD_SDL_FRONTEND` CMake target;
- `fellowng-sdl` executable linked with SDL3 and `FellowNG.Core.Portable`;
- resizable SDL3 application window;
- SDL event loop with clean quit handling;
- `--self-test` mode using a hidden window for CI qualification;
- dedicated Linux GitHub Actions workflow using SDL 3.4.16.

The normal executable is interactive and visible. The hidden window is used only by the automated CI self-test.

## Remaining M4 work

### M4.2 — video backend

Implement `IVideoOutput` using SDL3 textures/renderer and validate XRGB8888 frame presentation.

### M4.3 — input backend

Implement `IInputSource` for keyboard, mouse, gamepad/joystick and quit events.

### M4.4 — audio backend

Implement `IAudioOutput` for interleaved signed 16-bit PCM.

### M4.5 — Fellow integration

Connect the SDL backends to the actual Fellow runtime, boot a representative classic Amiga configuration interactively, and document remaining platform differences.
