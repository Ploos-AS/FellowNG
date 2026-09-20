# M7.3 Independent emulator cross-check

FellowNG qualification is complemented by an independent UAE implementation. The first reference target is FS-UAE/WinUAE lineage, used only as an external behavioral cross-check; FellowNG is not declared correct merely because another emulator agrees.

## Probe contract

The cross-check uses the same redistributable AROS m68k boot assets where possible and records:
- profile id and revision;
- emulator name/version and exact invocation;
- CPU/chipset/memory configuration;
- whether the guest reaches the expected boot stage;
- framebuffer dimensions and observable progress when available;
- exit/timeout status;
- host architecture;
- differences and limitations.

Initial probes:
1. reset/early boot — guest executes beyond reset without immediate exception or emulator termination;
2. sustained boot — guest continues executing for a selected bounded interval;
3. desktop progress — the AROS desktop profile produces visible framebuffer progress.

These are compatibility probes, not instruction-by-instruction equivalence tests.

## Reference records

Cross-check records belong under tests/crosscheck/ and contain the exact emulator version and command line. No copyrighted ROM or OS media is committed.

Statuses: pass, fail, unsupported, not-run. unsupported and not-run must never silently become pass.

## Initial implementation

The first automated integration should use FS-UAE where a reproducible Linux CI installation is available. Until that is established, the cross-check remains an explicit external qualification tool rather than a mandatory GitHub gate.