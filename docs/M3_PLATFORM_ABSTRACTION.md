# M3 — Platform abstraction

M3 is being delivered incrementally. The first slice establishes host-service boundaries that compile on Linux and Windows without Win32 APIs.

## M3.1 — Host services foundation

Implemented:

- `Platform/IClock.h`: monotonic clock and sleep abstraction.
- `Platform/StdClock.*`: portable C++20 implementation using `std::chrono::steady_clock` and `std::this_thread::sleep_for`.
- `Platform/IFileSystem.h`: file metadata/path abstraction.
- `Platform/StdFileSystem.*`: portable implementation using `std::filesystem` and `std::error_code`.
- Both implementations are part of `FellowNG.Core.Portable` and exercised by the portable-core smoke test.

The intent is to migrate existing host calls behind these boundaries gradually rather than rewrite emulation code.

## Remaining M3 work

### M3.2 — logging and host lifecycle

- Define portable logging sink/interface.
- Decouple core logging from Windows-specific presentation and debugger facilities.
- Define host lifecycle/service aggregation where useful.

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
