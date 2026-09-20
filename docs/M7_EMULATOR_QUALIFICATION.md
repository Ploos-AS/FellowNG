# M7 Emulator qualification

M7 turns FellowNG's portable runtime into a reproducible compatibility target. The
goal is not to claim perfect emulation or to hide differences from other
emulators, but to make supported machine profiles and observed behavior explicit.

## M7.1 — Qualification profile format

Each profile describes a classic Amiga configuration independently of copyrighted
ROM or OS media. Profiles are intended for local runs, CI with redistributable
AROS assets, and external runners such as `amiga-runtime`.

Profiles are split into two families. Classic/reference profiles describe
historical Amiga configurations and require user-supplied licensed ROM/OS media.
CI profiles use redistributable AROS m68k assets and are the authoritative public
GitHub Actions qualification path.

### Classic/reference profiles

| Profile | CPU | Chipset | Chip RAM | Fast RAM | Purpose |
| --- | --- | --- | --- | --- | --- |
| A500-1.x | 68000 | OCS | 512 KiB | 0 | baseline OCS/68000 |
| A500Plus-2.x | 68000 | ECS | 1 MiB | 0 | ECS baseline |
| A1200-020 | 68020 | AGA | 2 MiB | 8 MiB | representative AGA system |

### AROS/CI profiles

The first CI profile is derived from the configuration already qualified by the
M4 workflow rather than assuming a one-to-one AROS equivalent for every classic
machine.

| Profile | CPU | Boot assets | Qualification | Purpose |
| --- | --- | --- | --- | --- |
| AROS-m68k-020 | 68020 | official AROS m68k ROM + extended ROM | runtime boot/deep boot | redistributable core CI baseline |
| AROS-m68k-020-desktop | 68020 | official AROS m68k ROM + extended ROM + boot ADF + Live CD root | desktop framebuffer progress | redistributable end-to-end CI baseline |

The AROS asset fetcher is `tools/fetch_aros_m68k.py`. The existing M4 CI is the
reference for asset provenance and known-working Fellow arguments. Additional
AROS profiles must first demonstrate a working configuration before becoming
qualification baselines.

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

M7 is complete when the AROS/CI profiles have deterministic public FellowNG qualification, the classic/reference profiles have documented local qualification paths, selected probes have an independently recorded cross-check, and the compatibility report is reproducible from documented inputs.
