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

## M4 — SDL frontend ✅

- [x] M4.1: SDL3 build/bootstrap, visible window/event loop, and CI self-test.
- [x] M4.2: SDL3 video backend implementing `IVideoOutput` with XRGB8888 streaming textures.
- [x] M4.3: SDL3 keyboard/mouse/gamepad backend implementing `IInputSource`.
- [x] M4.4: SDL3 audio backend implementing `IAudioOutput` for interleaved signed 16-bit PCM.
- [x] M4.5: connect SDL backends to the Fellow runtime and boot a representative Amiga configuration interactively.
  - [x] M4.5a: portable emulator-runtime contract and frontend session/lifecycle pump.
  - [x] M4.5b: detach Win32 host-service/driver construction behind an injectable platform factory while preserving the Windows default backend.
  - [x] M4.5c: implement `WinFellowRuntime` behind `IEmulatorRuntime`, with injectable module startup/shutdown callbacks and bounded frontend-safe execution slices.
  - [x] M4.5d: wire SDL video/input/audio to the real Fellow runtime, replace the conservative instruction slice with a practical bounded scheduler slice, and complete an interactive Amiga boot.
    - [x] M4.5d94: qualify the real portable Linux/SDL3 runtime with redistributable AROS m68k ROMs in CI (start → bounded CPU pump → stop), without storing copyrighted Amiga ROM/OS assets.
    - [x] M4.5d95: activate explicit portable ROM configuration after module startup while preserving the intentional no-ROM smoke rejection path.
    - [x] M4.5d96: extend AROS qualification from bounded runtime smoke to sustained execution (1,048,576 runtime slices).
    - [x] M4.5d97: require observable portable framebuffer progress during the sustained AROS m68k run.
    - [x] M4.5d98: deepen AROS m68k boot qualification to 16,777,216 runtime slices and require multiple framebuffer state changes.
    - [x] M4.5d99: archive the final emulated framebuffer as CI evidence; qualified output reaches the AROS graphical boot splash at 640x400.
    - [x] M4.5d100: extend qualification beyond the AROS splash toward a usable Workbench/Desktop state and define a deterministic functional boot-state check. AROS Live CD filesystem/autoconfig boot now reaches qualified visible desktop progress in CI (>=16 changed frames and >=1024 non-background pixels), with framebuffer evidence archived. Guest 68k RESET no longer rebuilds host filesystem/autoconfig state.

See `docs/M4_SDL_FRONTEND.md`.

## M5 — CLI and automation ✅

- [x] M5.1: add stable CLI discovery through `--help` and `--version`.
- [x] M5.2: add machine-readable `--result-json` boot results using `fellowng.runtime-result.v1`.
- [x] M5.3: qualify the CLI and successful JSON result contract in CI.
- [x] M5.4–M5.6: emit and require machine-readable failure results.
- [x] M5.7–M5.9: document the automation interface, formalize its JSON schema, and validate emitted fields against it in CI.
- [x] M5.10–M5.12: add deterministic `--max-pumps N` execution bounds, keep frontend-only options out of the Fellow argument parser, and qualify the failure contract with a real AROS boot.
- [x] M5.13: publish a concise CLI/automation integration guide for external runners such as amiga-runtime. See `docs/M5_CLI_AUTOMATION.md`.
- [x] M5.14: complete M5 qualification and freeze the `fellowng.runtime-result.v1` automation contract. The final M5 CI baseline passes Linux SDL3/AROS runtime qualification, portable-core checks, repository checks, and Windows MSBuild.

## M6 — Cross-platform expansion ✅

- [x] M6.1: validate Windows through both the legacy frontend and portable SDL3 frontend. The portable frontend builds on Windows CI and passes CLI discovery, frontend self-test (using SDL dummy audio on the headless runner), and portable CTest coverage while legacy MSBuild remains green.
- [x] M6.2: add macOS portable SDL3 build support and qualify CLI/self-test behavior. Native Apple Silicon (arm64) CI builds the portable frontend and passes CLI discovery, SDL3 self-test, and portable CTest coverage.
- [x] M6.3: validate native ARM64 Linux support with reproducible SDL3 build/test coverage. GitHub Actions `ubuntu-24.04-arm` builds the portable frontend and passes CLI discovery, headless SDL3 self-test, and portable CTest coverage.

## M7 — Emulator qualification ✅

- [x] M7.1: define separate classic/reference and redistributable AROS/CI qualification profile families; seed classic A500/A500+/A1200 profiles and known-working AROS m68k 68020 boot/desktop baselines.
- [x] M7.2: implement deterministic FellowNG runs for the initial AROS/CI profiles using the frozen runtime-result v1 contract. Both the boot/deep-boot and desktop profiles pass end-to-end in public GitHub Actions through the machine-readable profile runner.
- [x] M7.3: cross-check the initial AROS sustained-boot probe against FS-UAE in public GitHub Actions. The job uses the same official AROS m68k assets, native FS-UAE 3.1.66, a dedicated Xvfb display, and archived boot-frame/log evidence. This establishes an external runtime/display baseline; deeper semantic probe comparison remains part of M7.4.
- [x] M7.4: publish a reproducible compatibility matrix including the public AROS qualification baselines, classic/local profiles, independent FS-UAE evidence, and explicitly documented timing/semantic limitations. See `docs/M7_COMPATIBILITY_MATRIX.md`.

See `docs/M7_EMULATOR_QUALIFICATION.md`.

## M8 — Production usability and runtime integration 🚧

M8 turns the qualified emulator into a practical backend for day-to-day development and automated Amiga software qualification.

- [x] **M8.1 — amiga-runtime adapter:** define and document the FellowNG invocation/profile/result mapping required by external runtime orchestration while keeping `fellowng.runtime-result.v1` stable.
- [x] **M8.2 — classic local qualification:** classic A500 1.x, A500+ 2.x, and A1200 3.x reference profiles are wired through the common local qualification/evidence runner for user-supplied licensed assets; no copyrighted ROM or OS assets are stored in the repository.
- [x] **M8.3 — ARexx m68k qualification:** deterministic guest-side m68k ARexx probe, staging workflow, and `fellowng.arexx-result.v1` machine-readable pass/fail evidence are implemented for cross-emulator qualification.
- [x] **M8.4 — runtime evidence contract:** standardize logs, screenshots/framebuffers, profile metadata, exit status, and JSON results so external runners can archive comparable evidence.
- [x] **M8.5 — configuration usability:** improve portable profile/config loading and diagnostics for interactive users without weakening deterministic automation.
- [x] **M8.6 — release readiness:** establish versioned portable build artifacts and release checks for the supported host matrix.
- [x] **M8.7 — integration qualification:** FellowNG passed the external `amiga-runtime` GitHub Actions integration in run #357 (2026-09-26), including backend discovery, deterministic AROS m68k Q3 boot qualification, emulator-neutral dispatch, and the four-backend FS-UAE/Amiberry/FellowNG/Copperline comparison.
- [ ] **M8.8 — complete M8:** freeze the first production-integration baseline and publish its qualification matrix.

See `docs/M8_PRODUCTION_INTEGRATION.md`.

## Principles

1. Preserve Fellow's independent emulator lineage.
2. Do not rewrite working emulation code merely for style.
3. Keep upstream attribution and GPLv2 obligations explicit.
4. Prefer incremental, testable portability work.
5. Keep legacy Windows support working while the portable path matures.
6. Treat deterministic automation as a first-class feature, not an afterthought.
