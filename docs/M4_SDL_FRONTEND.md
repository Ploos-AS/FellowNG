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

## M4.2 — video backend ✅

Implemented:

- `SdlVideoOutput` implementing `Platform::IVideoOutput`;
- SDL3 renderer + streaming texture backend;
- native `PixelFormat::Xrgb8888` upload path;
- texture recreation when incoming frame dimensions change;
- nearest-neighbor texture scaling for pixel-accurate classic output;
- frame validation for dimensions, pitch and buffer size;
- `--self-test` now creates and presents a generated 320×256 XRGB8888 frame before reporting PASS.

The renderer stretches the source frame to the current SDL window size, so normal interactive execution remains resize-safe. Aspect-ratio policy is intentionally deferred until the Fellow runtime supplies the exact display-mode requirements.

## Remaining M4 work

### M4.3 — input backend

Implement `IInputSource` for keyboard, mouse, gamepad/joystick and quit events.

### M4.4 — audio backend

Implement `IAudioOutput` for interleaved signed 16-bit PCM.

### M4.5 — Fellow integration

Connect the SDL backends to the actual Fellow runtime, boot a representative classic Amiga configuration interactively, and document remaining platform differences.
