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

### M4.5b — portable WinFellow runtime adapter

Remaining work:

- split Win32-only service/driver creation out of `VirtualHost/CoreFactory.cpp`;
- provide portable replacements/adapters for logging, file operations, HUD/requester behavior, sound-driver selection, and host lifecycle;
- expose the existing Fellow startup/emulation/shutdown sequence behind `IEmulatorRuntime`;
- preserve the current Windows/DirectX path while adding the portable path;
- arrange bounded or worker-thread execution so SDL can continue polling input while Fellow runs.

### M4.5c — interactive Amiga boot

After M4.5b, wire the real renderer, keyboard/gameport input, and sound generator to the SDL backends and boot a representative classic Amiga configuration. ROMs and Amiga OS media remain external user-supplied assets and are never distributed by FellowNG.
