# FellowNG Roadmap

FellowNG is a modern, cross-platform continuation of the Fellow/WinFellow Amiga emulator.

The project starts from the existing WinFellow source tree and preserves its independent emulation lineage while progressively separating portable emulator code from platform-specific frontends.

## M0 — Foundation ✅

- Establish FellowNG project identity and scope.
- Preserve upstream WinFellow source and GPLv2 licensing.
- Document upstream provenance and compatibility goals.
- Keep the existing Windows build green.
- Define the portability architecture and milestone sequence.
- Add a lightweight repository-level M0 validation workflow.

## M1 — Portability inventory ✅

- Inventory Win32/DirectX dependencies.
- Classify code into emulator core, platform abstraction, frontend, tooling, and tests.
- Identify compiler- and ABI-specific assumptions.
- Produce an initial Linux portability report.

See `docs/M1_PORTABILITY_INVENTORY.md`.

## M2 — Portable core build ✅

- Introduce a CMake-based portable build for reusable core components.
- Build an initial real Fellow core slice and smoke test on Linux x86-64.
- Keep the native Visual Studio build untouched.
- No GUI requirement yet.

The initial portable slice contains the custom-chipset register model and register utility. It builds as `FellowNG.Core.Portable` and is exercised through CTest on Ubuntu CI.

See `docs/M2_PORTABLE_CORE.md`.

## M3 — Platform abstraction ✅

- [x] M3.1: establish portable timing and filesystem/path host-service boundaries.
- [x] M3.2: portable logging and host lifecycle/service aggregation.
- [x] M3.3: threading and synchronization abstraction.
- [x] M3.4: explicit input, audio, and video frontend interfaces while retaining Win32/DirectX backends.

M3 provides the portable host contracts required for an SDL frontend without rewriting the Fellow emulation core.

See `docs/M3_PLATFORM_ABSTRACTION.md`.

## M4 — SDL frontend

- Add an SDL3 frontend for Linux.
- Implement display, keyboard, mouse, joystick, and audio paths.
- Boot a basic Amiga configuration interactively.

## M5 — CLI and automation

- Add stable command-line configuration suitable for CI and scripted use.
- Add deterministic startup/exit behavior.
- Add headless or automation-oriented execution where technically practical.
- Produce machine-readable runtime results.

## M6 — Cross-platform expansion

- Validate Windows through both the legacy frontend and portable frontend where applicable.
- Add macOS build support.
- Evaluate ARM64 Linux support.

## M7 — Emulator qualification

- Add reproducible compatibility profiles for representative classic Amiga systems.
- Cross-check selected tests against independent emulator implementations.
- Document timing and compatibility differences rather than hiding them.

## Principles

1. Preserve Fellow's independent emulator lineage.
2. Do not rewrite working emulation code merely for style.
3. Keep upstream attribution and GPLv2 obligations explicit.
4. Prefer incremental, testable portability work.
5. Keep legacy Windows support working while the portable path matures.
6. Treat deterministic automation as a first-class feature, not an afterthought.
