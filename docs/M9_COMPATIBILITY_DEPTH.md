# M9 — Compatibility depth and real workloads

M9 starts from the completed M8 production-integration baseline. Its purpose is to turn broad boot/runtime qualification into focused, reproducible evidence about the parts of the Amiga that real software depends on.

## Baseline policy

M8 remains frozen as the production integration boundary. M9 may add new probe/result schemas, profiles, workloads, and evidence, but it must not silently change the semantics of `fellowng.runtime-result.v1`.

Public CI uses only redistributable inputs. Copyrighted Kickstart ROMs, Workbench/AmigaOS media, commercial applications, games, and demos are never committed. A separate local path may consume user-supplied licensed assets.

Cross-emulator testing is comparative evidence, not an assumption that any one emulator is the correctness oracle.

## M9.1 — compatibility probe framework

The first step is a small versioned probe framework that can express a test identity, subsystem, machine/profile requirements, deterministic execution bound, observations, PASS/FAIL/SKIP result, and evidence references.

Initial subsystem vocabulary:

- CPU / exceptions / interrupts
- custom chipset / DMA
- CIA and timers
- input
- floppy and hardfile storage
- audio
- OS/runtime semantics
- application workload

Probe payloads should be project-authored or otherwise redistributable, small enough to diagnose failures, and runnable both directly in FellowNG and through external orchestration where practical.

## Completion criteria

M9 is complete when representative redistributable probes cover the major subsystems, stable probes run as regression gates, applicable evidence can be compared across emulator backends, classic/local execution is documented, and the compatibility matrix records both successful behavior and known differences.
