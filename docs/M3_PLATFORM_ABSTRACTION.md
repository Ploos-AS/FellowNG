# M3 — Platform abstraction

M3 establishes host-service boundaries that compile on Linux and Windows without Win32 APIs, while preserving the existing WinFellow backends until portable replacements are ready.

## M3.1 — Host services foundation ✅

Implemented:

- `Platform/IClock.h`: monotonic clock and sleep abstraction.
- `Platform/StdClock.*`: portable C++20 implementation using `std::chrono::steady_clock` and `std::this_thread::sleep_for`.
- `Platform/IFileSystem.h`: file metadata/path abstraction.
- `Platform/StdFileSystem.*`: portable implementation using `std::filesystem` and `std::error_code`.
- Both implementations are part of `FellowNG.Core.Portable` and exercised by the portable-core smoke test.

## M3.2 — Logging and host lifecycle ✅

Implemented:

- `Platform/ILogger.h`: portable logging boundary with explicit log levels.
- `Platform/StreamLogger.*`: thread-safe standard C++ stream-backed logger.
- `Platform/IHostLifecycle.h`: explicit host start/stop/running contract.
- `Platform/HostLifecycle.*`: portable lifecycle implementation with idempotent start/stop behavior and lifecycle logging.
- Portable-core smoke coverage verifies lifecycle state transitions and emitted log messages.

The legacy `Service::Log` remains untouched for the existing WinFellow frontend. It currently contains MSVC/Windows-specific formatting and time helpers (`vsprintf_s`, `_snprintf`, `_vsnprintf`, `localtime_s`, `fopen_s`). Later migration can adapt that behavior behind the new logger boundary rather than modify emulator behavior during this milestone.

## M3.3 — Threading and synchronization ✅

Implemented:

- `Platform/IThread.h`: minimal host-thread contract with explicit start/join semantics.
- `Platform/StdThread.*`: C++20 backend using `std::thread`; destruction safely joins an outstanding worker.
- `Platform/IEvent.h`: signal/reset/wait contract suitable for event-style synchronization.
- `Platform/ManualResetEvent.*`: manual-reset event semantics using `std::mutex` and `std::condition_variable`.
- Portable-core smoke coverage verifies blocking wake-up, timeout, persistent signaled state, reset behavior, joinability, and prevention of a second concurrent `Start()` on the same thread object.

This milestone deliberately does not replace every Win32 synchronization call mechanically. Existing code must be migrated only when its required semantics are understood. In particular, timing-sensitive or auto-reset-event behavior must remain explicit rather than being approximated silently.

## M3.4 — Frontend boundaries ✅

Implemented:

- `Platform/IAudioOutput.h`: backend-neutral PCM output boundary using interleaved signed 16-bit samples and an explicit sample-rate/channel format.
- `Platform/IVideoOutput.h`: frame-presentation boundary with explicit dimensions, pitch, pixel format, and byte span.
- `Platform/IInputSource.h`: normalized polling boundary for keyboard, mouse, joystick, and quit events.
- Portable-core smoke coverage provides small test backends and verifies start/stop, audio submission, frame presentation, and input polling contracts.

The existing WinFellow `ISoundDriver` and Win32/DirectX input/video/audio code are intentionally retained. M3 defines the portable contracts; adapters from the legacy Windows backend and new SDL3 implementations belong to later work.

The initial video contract uses `Xrgb8888` as the first portable pixel format. Additional formats should be added only when a backend or emulation path requires them.

## M3 result

M3 now provides portable boundaries for:

- timing and sleep
- filesystem and paths
- logging
- host lifecycle
- threads and event synchronization
- audio output
- video presentation
- keyboard/mouse/joystick input

This is enough architectural separation to begin M4 without rewriting the Fellow emulation core or deleting the working Windows frontend.

## Rule

Platform abstraction must not silently change Amiga-visible timing or device semantics. Where host behavior affects emulation correctness, tests and documentation take precedence over cosmetic refactoring.
