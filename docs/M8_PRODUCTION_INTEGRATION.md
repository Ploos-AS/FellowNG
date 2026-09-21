# M8 — Production usability and runtime integration

M8 begins after completion of the M0–M7 modernization and emulator-qualification sequence. Its purpose is to make FellowNG a practical, reproducible backend for interactive use and external Amiga runtime qualification.

## Scope

M8 focuses on integration and usability rather than a new emulator-core rewrite. The frozen `fellowng.runtime-result.v1` result contract remains the compatibility boundary unless a separately versioned successor is justified.

### M8.1 — amiga-runtime adapter

Define the command-line, profile, asset, timeout/bound, result, and evidence mapping needed for an external runtime orchestrator to invoke FellowNG deterministically.

The integration must not require copyrighted Kickstart ROMs, AmigaOS/Workbench media, or other proprietary assets to be committed to FellowNG.

### M8.2 — classic local qualification

Use the existing classic/reference profile family to qualify representative configurations with user-supplied assets:

- A500 / Kickstart 1.x
- A500+ / Kickstart 2.x
- A1200 / 68020 / Kickstart 3.x

Public CI continues to use redistributable assets only.

### M8.3 — ARexx m68k qualification

Add a guest-side ARexx probe executed on the emulated m68k environment. The probe must provide deterministic pass/fail evidence and must not substitute an x86 AROS-host ARexx implementation for the guest m68k environment.

### M8.4 — runtime evidence contract

Standardize the evidence produced by qualification runs: runtime-result JSON, emulator/profile identity, logs, final framebuffer or screenshot when applicable, execution bounds, and exit status.

### M8.5 — configuration usability

Make portable configuration/profile selection and diagnostics convenient enough for normal interactive use while preserving the deterministic CLI path used by CI.

### M8.6 — release readiness

Produce versioned portable artifacts and release checks for supported hosts. Release work must retain the legacy Windows baseline until the portable frontend has an explicitly documented replacement policy.

### M8.7 — integration qualification

Run FellowNG through the external amiga-runtime integration and record reproducible qualification results, including known limitations and differences from other emulator backends.

### M8.8 — completion gate

M8 is complete when the production-integration baseline is reproducible, documented, externally consumable, and represented in an updated compatibility matrix.

## Constraints

- Do not store Kickstart ROMs or AmigaOS/Workbench media in Git.
- Preserve Fellow/WinFellow GPLv2 provenance and applicable upstream notices.
- Preserve Fellow's independent emulator lineage.
- Prefer deterministic, machine-readable qualification over screenshot-only success criteria.
- Keep public CI reproducible with legally redistributable assets.
