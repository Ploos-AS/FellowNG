![WinFellow](./fellow/Docs/WinFellow/winfellow_logo_large.png)

# FellowNG

**A modern, cross-platform continuation of the Fellow/WinFellow Amiga emulator.**

FellowNG starts from the existing WinFellow source tree and aims to make Fellow a first-class emulator on modern platforms while preserving its independent emulation lineage.

The immediate focus is portability: separate the reusable emulator core from Windows-specific host code, establish a portable build, add a modern SDL-based frontend, and make deterministic command-line automation a first-class capability.

> **Project status:** M4.5 — SDL3 runtime integration. Linux now builds and links the real Fellow runtime through the portable SDL3 frontend. CI qualifies startup, bounded CPU execution, and shutdown with official redistributable AROS m68k ROMs; sustained interactive boot qualification is the next step.

## Why FellowNG?

Most widely used modern Amiga emulators belong to the UAE family. Fellow is valuable because its emulator core has a separate lineage. Keeping Fellow healthy provides another implementation for users, developers, compatibility testing, and independent cross-validation of Amiga software.

FellowNG is an incremental continuation, not a rewrite. Working emulation code should be preserved unless there is a concrete portability, correctness, maintainability, or testing reason to change it.

## Goals

- Preserve and modernize the Fellow emulator.
- Make Linux a first-class supported host platform.
- Retain Windows support throughout the transition.
- Add macOS and additional architectures when the portable foundation is ready.
- Separate emulator core, host/platform services, and frontend code.
- Introduce a portable build system alongside the existing Visual Studio build.
- Add an SDL3 frontend for portable video, audio, and input.
- Provide stable CLI and automation interfaces suitable for CI.
- Support reproducible emulator profiles and runtime qualification.
- Keep Fellow useful as an implementation independent from the UAE emulator family.

## Non-goals for the early milestones

- Rewriting the emulator core from scratch.
- Chasing every WinUAE feature before portability is established.
- Shipping copyrighted Kickstart ROMs, Workbench media, or commercial software.
- Removing the existing Windows frontend before its replacement is mature.

## Current source tree

The imported WinFellow source lives primarily under:

```text
fellow/SRC
```

The generic emulation engine and Windows-specific implementations are currently intermingled in the historical layout. M1 will inventory these dependencies before structural changes begin.

The existing Visual Studio solution is:

```text
fellow/SRC/WinFellow.sln
```

## Building the current baseline

The current functional baseline is still WinFellow and requires Microsoft Visual Studio 2022. GitHub Actions also builds this baseline and runs the existing core and hardfile unit tests.

Portable Linux build instructions will be added when the M2 portable-core build exists.

## Roadmap

See [ROADMAP.md](ROADMAP.md).

The planned progression is:

**M0 foundation → M1 portability inventory → M2 portable core → M3 platform abstraction → M4 SDL frontend → M5 CLI/automation → M6 cross-platform expansion → M7 emulator qualification.**

## Upstream and provenance

FellowNG is based on WinFellow:

https://github.com/petschau/WinFellow

See [docs/UPSTREAM.md](docs/UPSTREAM.md) for provenance and contribution policy.

The historical WinFellow tree includes a filesystem module derived from WinUAE plus supporting UAE-derived files. Those components retain their upstream provenance; Fellow itself has an independent emulator lineage.

## License

FellowNG inherits the **GNU General Public License version 2 (GPLv2)** from WinFellow. Existing copyright and attribution notices must be preserved.

See [LICENSE](LICENSE).

## Contributing

FellowNG favors small, reviewable portability changes that keep the existing baseline working. See [CONTRIBUTING.md](CONTRIBUTING.md).

## Project

FellowNG is maintained under the Ploos-AS organization as an open-source Amiga preservation, development, and compatibility project.
