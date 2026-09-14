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
- `--self-test` creates and presents a generated 320×256 XRGB8888 frame.

The renderer stretches the source frame to the current SDL window size, so normal interactive execution remains resize-safe. Aspect-ratio policy is intentionally deferred until the Fellow runtime supplies the exact display-mode requirements.

## M4.3 — input backend ✅

Implemented:

- `SdlInputSource` implementing `Platform::IInputSource`;
- SDL3 keyboard down/up translation using scancodes;
- relative mouse movement and mouse-button events;
- gamepad axis and button translation into the portable joystick event types;
- SDL quit translation into `InputType::Quit`;
- normal interactive event processing routed through the portable input interface;
- CI self-test pushes synthetic keyboard, mouse, gamepad-axis and quit SDL events and validates the translated portable events.

SDL device identifiers are retained in the portable event value fields where useful so later Fellow integration can select or route physical controllers without coupling the core to SDL types.

## M4.4 — audio backend ✅

Implemented:

- `SdlAudioOutput` implementing `Platform::IAudioOutput`;
- interleaved signed 16-bit PCM input via `SDL_AUDIO_S16`;
- configurable sample rate and channel count from the portable `AudioFormat` contract;
- SDL3 playback stream creation through the default playback device;
- explicit stream-device resume after opening;
- queued PCM submission with `SDL_PutAudioStreamData`;
- deterministic stream shutdown through `SDL_DestroyAudioStream`;
- CI self-test opens the SDL dummy audio backend, queues a stereo 44.1 kHz silence buffer, and verifies start/submit/stop state transitions.

M4.4 intentionally does not connect Fellow's legacy sound generator yet. That wiring belongs to M4.5 so the SDL audio backend can first be qualified independently of emulator-runtime behavior.

## M4.5 — Fellow integration

### M4.5a — runtime bridge ✅

Implemented:

- `Platform::IEmulatorRuntime`, a frontend-neutral contract for runtime start/stop, input delivery, and bounded runtime execution through `RunSlice()`;
- `Platform::FrontendSession`, which owns the frontend-side orchestration between `IInputSource`, `IVideoOutput`, `IAudioOutput`, and the emulator runtime;
- quit handling is centralized in `FrontendSession` rather than being SDL-specific;
- a portable `runtime-session-smoke` CTest target validates start, input forwarding, runtime slicing, and deterministic stop behavior.

This seam is necessary because the current WinFellow `fellowRun()` path is blocking and still coupled to the Windows GUI lifecycle. The SDL frontend must not directly call that loop from its UI/event thread.

### M4.5b — host platform factory split ✅

Implemented:

- `VirtualHost/ICorePlatformFactory.h` defines creation hooks for the host-dependent sound driver, file operations, HUD, and RetroPlatform service;
- `CoreFactory` now accepts an injected `ICorePlatformFactory` through `SetPlatformFactory()` and exposes the selected provider through `GetPlatformFactory()`;
- the existing DirectSound, `FileopsWin32`, Windows HUD, and `RetroPlatformWrapper` construction lives behind a Windows default factory, preserving existing WinFellow behavior;
- generic services such as `Log` and `FileInformation` remain owned by `CoreFactory` rather than being unnecessarily platform-specific;
- a Linux portable-core compile smoke test includes the new factory contract to catch accidental Win32 leakage at the interface boundary.

This does not yet make the complete WinFellow runtime linkable on Linux. It removes the first explicit host-construction barrier so a future SDL/Linux provider can replace Windows services without changing the emulator's global `Core` shape.

### M4.5c — WinFellow lifecycle adapter

Next work:

- extract the current module startup/shutdown sequence from the Windows UI entry path;
- expose startup, emulation start/stop, and shutdown behind `Platform::IEmulatorRuntime`;
- keep `wguiStartup()`/`wguiShutdown()` optional so the SDL runtime does not instantiate the Windows GUI;
- make the blocking `fellowRun()`/`busRun()` execution frontend-safe, most likely through a dedicated emulator worker thread with controlled stop/join behavior;
- preserve the existing Windows executable path unchanged.

### M4.5d — interactive Amiga boot

After M4.5c, wire the real renderer, keyboard/gameport input, and sound generator to the SDL backends and boot a representative classic Amiga configuration. ROMs and Amiga OS media remain external user-supplied assets and are never distributed by FellowNG.
