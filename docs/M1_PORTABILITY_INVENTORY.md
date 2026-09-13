# M1 Portability Inventory

Status: **COMPLETE**

This document records the initial portability inventory for FellowNG. It is intentionally architectural: M1 identifies boundaries and risks before the first Linux build work begins in M2.

## Executive summary

The imported WinFellow tree is not a monolithic Win32-only codebase. It already contains separable projects for core emulator code, hardfile support, tests, generated 68k tooling, and the legacy WinFellow application. This gives FellowNG a useful starting point for a staged cross-platform port.

The primary portability obstacle is the legacy WinFellow application/frontend. Its Visual Studio project is explicitly Windows-oriented and currently links host APIs including DirectDraw, DirectInput 8, DirectSound, DXGI, D3D11, WinMM, Comctl32 and Shlwapi.

The M2 strategy is therefore **portable-core-first**, not "port the GUI first".

## Source-tree classification

### Candidate portable core

- `fellow/SRC/WinFellow.Core/`
- `fellow/SRC/WinFellow.Hardfile/`
- portions of `fellow/SRC/WinFellow/C/`
- portions of `fellow/SRC/WinFellow/INCLUDE/`
- CPU generation / support code where host assumptions permit

These areas are the first candidates for a CMake-based Linux x86-64 build.

### Tests and validation

- `fellow/SRC/WinFellow.Core.Tests/`
- `fellow/SRC/WinFellow.Hardfile.Tests/`
- `fellow/SRC/WinFellow.Test.Infrastructure/`
- `fellow/SRC/M68KTester/`

Existing tests are valuable because M2 can prove portability without requiring a working Linux GUI.

### Legacy Windows frontend / host integration

- `fellow/SRC/WinFellow/Windows/`
- `fellow/SRC/WinFellow/DXGI/`
- Win32 resource files (`*.rc`)
- Visual Studio project/solution integration
- Windows-specific application lifecycle and UI code

This layer should remain supported while FellowNG introduces portable host abstractions.

### Bundled / compatibility components

- `fellow/SRC/WinFellow/uae/`
- `fellow/SRC/WinFellow/IPF/`
- `fellow/SRC/WinFellow/xdms/`
- `fellow/SRC/WinFellow/zlib/`

These require individual licensing/build treatment but should not be conflated with the Fellow emulator core.

## Confirmed Windows build dependencies

The current WinFellow Visual Studio application links the following notable Windows libraries:

- `ddraw.lib` — DirectDraw video path
- `dinput8.lib` — DirectInput input path
- `dsound.lib` — DirectSound audio path
- `dxgi.lib` — DXGI display infrastructure
- `d3d11.lib` — Direct3D 11 rendering path
- `winmm.lib` — multimedia/timing support
- `comctl32.lib` — common controls / GUI
- `shlwapi.lib` — Windows shell helpers
- `odbc32.lib`, `odbccp32.lib` — currently present in linker dependencies and must be verified for actual necessity

The project also uses Windows resource compilation, a Windows subsystem executable, Visual Studio toolsets, and PowerShell-based pre-build version generation.

## Portability boundaries to introduce

FellowNG should eventually expose explicit host interfaces for:

1. **Video** — framebuffer/presentation/fullscreen/window handling.
2. **Audio** — sample output, buffering and device lifecycle.
3. **Input** — keyboard, mouse, joystick/game controller.
4. **Timing** — monotonic clock, sleeps, high-resolution timing and frame pacing.
5. **Threading/synchronization** — threads, locks, events where required.
6. **Filesystem/paths** — host paths, configuration locations and file dialogs.
7. **Application lifecycle** — startup, shutdown, reset and error reporting.
8. **Dynamic/optional components** — IPF or other host-loaded modules.
9. **Logging/diagnostics** — portable diagnostic sink usable by GUI and CI frontends.

M3 owns the formal host-abstraction layer. M2 should avoid designing all of it prematurely; it should introduce only the minimum seams needed to compile/test portable components.

## Compiler and ABI risks

The first Linux build must explicitly check for:

- MSVC-specific keywords/macros and warning pragmas.
- Windows scalar and handle types leaking into otherwise generic headers.
- assumptions about `__fastcall` / calling convention settings.
- pointer-size and integer-width assumptions.
- little-endian host assumptions.
- structure packing / alignment dependencies.
- Windows path separators and drive-letter assumptions.
- case-insensitive filesystem assumptions.
- CRT-specific names or secure-CRT replacements.
- generated code that assumes MSVC syntax or assembler conventions.

The existing Visual Studio project configures a fastcall calling convention for relevant configurations; this must not silently become part of a portable ABI contract.

## Linux target for M2

Initial M2 target:

- Linux x86-64
- GCC and/or Clang
- CMake
- no GUI requirement
- no audio/video/input requirement unless a core unit requires a stub
- build reusable core/hardfile components
- run every existing test that can be made host-neutral without changing emulator behavior

SDL3 is intentionally deferred to M4. M2 is successful when Fellow logic can compile and execute meaningful tests on Linux independently of the Win32 frontend.

## Non-goals for M1

M1 does **not**:

- claim that all files under `WinFellow.Core` are already portable;
- change emulator behavior;
- replace DirectX;
- introduce SDL;
- remove the Visual Studio build;
- promise ROM redistribution;
- claim runtime Amiga compatibility on Linux yet.

## M1 exit criteria

- [x] Source tree classified.
- [x] Major Windows/DirectX dependencies identified.
- [x] Candidate portable projects identified.
- [x] Compiler/ABI portability risks documented.
- [x] Linux M2 target defined.
- [x] Windows build retained as the regression baseline.

## Decision

Proceed to **M2 — Portable core build**.

M2 should start with a minimal top-level CMake build that proves selected host-neutral sources and tests compile on Linux, while leaving the existing Visual Studio solution untouched.
