# M7 Emulator qualification

M7 turns FellowNG's portable runtime into a reproducible compatibility target. The
goal is not to claim perfect emulation or to hide differences from other
emulators, but to make supported machine profiles and observed behavior explicit.

## M7.1 — Qualification profile format

Each profile describes a classic Amiga configuration independently of copyrighted
ROM or OS media. Profiles are intended for local runs, CI with redistributable
AROS assets, and external runners such as `amiga-runtime`.

Initial profile set:

| Profile | CPU | Chipset | Chip RAM | Fast RAM | Purpose |
| --- | --- | --- | --- | --- | --- |
| A500-1.x | 68000 | OCS | 512 KiB | 0 | baseline OCS/68000 |
| A500Plus-2.x | 68000 | ECS | 1 MiB | 0 | ECS baseline |
| A1200-020 | 68020 | AGA | 2 MiB | 8 MiB | representative AGA system |

A qualification record must capture at least:

- stable profile identifier and revision;
- CPU/chipset/memory configuration;
- boot/qualification mode and deterministic pump bound;
- runtime-result schema version;
- process exit status;
- framebuffer progress/signature data when applicable;
- ROM/media provenance as metadata, never bundled copyrighted content;
- host OS and architecture.

## M7.2 — Reproducible FellowNG runs

Translate each profile into deterministic FellowNG CLI configuration and qualify
it with `--result-json` and an explicit `--max-pumps`. Public CI uses only
redistributable assets. Licensed Commodore/Amiga assets remain user supplied.

## M7.3 — Independent emulator cross-check

Run selected profile probes against at least one independent emulator
implementation. Compare observable behavior rather than implementation details.
Record differences in boot behavior, chipset-visible output, CPU behavior and
timing-sensitive tests without treating either implementation as automatically
authoritative.

## M7.4 — Compatibility report

Publish the qualified profile matrix, known differences, unsupported features and
reproduction commands. Results should identify the exact FellowNG revision and
profile revision.

## Completion criteria

M7 is complete when the initial three profiles have deterministic FellowNG
qualification, selected probes have an independently recorded cross-check, and
the compatibility report is reproducible from documented inputs.
