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

### M4.5c — WinFellow lifecycle adapter ✅

Implemented:

- `FellowNG::Runtime::WinFellowRuntime` implements `Platform::IEmulatorRuntime` directly on top of the existing Fellow emulation start/stop and single-step APIs;
- startup/shutdown of the broader module graph is injectable through callback hooks, allowing the legacy Windows entry path to remain unchanged while a future SDL host supplies its own module lifecycle;
- the adapter records the frontend video/audio endpoints for the next wiring step without prematurely coupling legacy Fellow devices to SDL;
- runtime execution is bounded through the existing one-instruction stepping path instead of calling blocking `fellowRun()`/`busRun()` from the frontend thread;
- quit input requests a safe Fellow emulation stop;
- adapter shutdown is deterministic and idempotent, including destructor cleanup.

### M4.5d — interactive Amiga boot

Completed integration increments:

- portable renderer callback and SDL video bridge;
- portable keyboard, mouse and joystick input bridge;
- portable audio driver and audio-platform-factory decorator;
- bounded frontend scheduler;
- host platform factory split;
- portable runtime-factory contract;
- SDL frontend runtime ownership through `IEmulatorRuntimeFactory`;
- concrete `WinFellowRuntimeFactory` provider, allowing frontend code to request the real Fellow runtime through the portable factory contract without depending on the concrete runtime type.

### M4.5e2 — frontend-neutral module lifecycle ✅

Implemented:

- the canonical Fellow subsystem startup/shutdown sequence now runs through shared lifecycle helpers with optional frontend hooks;
- the legacy Windows path passes `wguiStartup()` and `wguiShutdown()` as hooks at exactly their previous positions in the lifecycle order;
- `fellowModulesStartupPortable()` and `fellowModulesShutdownPortable()` execute the same Fellow subsystem order without invoking Windows GUI startup or shutdown;
- portable graphics-start failure reporting logs to the Fellow log/stderr rather than entering the Windows requester path;
- `WinFellowRuntimeFactory` selects the portable module lifecycle by default, so runtime instances created for a non-Windows frontend no longer require callers to supply ad-hoc startup/shutdown callbacks.

Remaining work:

- provide an SDL/Linux core-platform provider for file operations, HUD and RetroPlatform services;
- link the required Fellow emulation sources into the SDL/Linux build;
- instantiate `WinFellowRuntimeFactory` from normal SDL execution;
- replace the conservative instruction stepping internals with a practical bounded Fellow scheduler slice;
- boot a representative classic Amiga configuration interactively.

ROMs and Amiga OS media remain external user-supplied assets and are never distributed by FellowNG.
