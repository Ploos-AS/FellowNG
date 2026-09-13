# M3 — Platform abstraction

M3 is being delivered incrementally. The goal is to establish host-service boundaries that compile on Linux and Windows without Win32 APIs, then move legacy host behavior behind those boundaries without changing Amiga-visible semantics.

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

## Remaining M3 work

### M3.3 — threading and synchronization

- Inventory Win32 thread/event/critical-section usage.
- Introduce portable primitives based on standard C++ where semantics match.
- Keep specialized low-level behavior explicit when standard primitives are not equivalent.

### M3.4 — frontend boundaries

Create explicit interfaces for:

- video/frame presentation
- keyboard/mouse/joystick input
- audio output

The existing Win32/DirectX implementations remain the Windows backend. SDL3 implementations are M4.

## Rule

Platform abstraction must not silently change Amiga-visible timing or device semantics. Where host behavior affects emulation correctness, tests and documentation take precedence over cosmetic refactoring.
