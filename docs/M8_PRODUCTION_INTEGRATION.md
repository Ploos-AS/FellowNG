# M8 — Production usability and runtime integration

M8 begins after completion of the M0–M7 modernization and emulator-qualification sequence. Its purpose is to make FellowNG a practical, reproducible backend for interactive use and external Amiga runtime qualification.

## Scope

M8 focuses on integration and usability rather than a new emulator-core rewrite. The frozen `fellowng.runtime-result.v1` result contract remains the compatibility boundary unless a separately versioned successor is justified.

### M8.1 — amiga-runtime adapter ✅

The FellowNG side of the external-runner contract is now defined. An orchestrator such as `amiga-runtime` should treat FellowNG as a process backend with the following mapping:

| Runner concept | FellowNG mapping |
| --- | --- |
| executable | `fellowng-sdl` |
| smoke/boot mode | `--runtime-boot` |
| sustained boot mode | `--runtime-boot-deep` |
| desktop mode | `--runtime-boot-desktop` |
| structured result | `--result-json` / `fellowng.runtime-result.v1` |
| deterministic bound | `--max-pumps N` |
| emulator configuration | normal Fellow options after frontend options |
| success/failure | process exit status plus runtime-result `status` |
| evidence | stdout, stderr, result JSON and framebuffer evidence where requested |

The external runner owns asset discovery and staging. ROM, filesystem and OS paths are passed to FellowNG through normal Fellow configuration options; they are never embedded in the adapter contract. Public automation must use redistributable assets. Local qualification may use user-supplied licensed assets.

The runner must select exactly one runtime boot mode, request JSON output, provide an explicit pump bound, impose an outer process timeout as a hang/crash guard, preserve stdout/stderr, validate the emitted result against `docs/schema/runtime-result-v1.schema.json`, and archive applicable framebuffer evidence.

Profile translation is intentionally explicit rather than inferred. The runner selects a FellowNG qualification profile or maps its own machine profile to equivalent Fellow configuration. Existing public baselines are `profiles/qualification/aros-m68k-020.json` and `profiles/qualification/aros-m68k-020-desktop.json`. Classic profiles remain local/reference profiles because their ROM/OS assets are not redistributable.

The `fellowng.runtime-result.v1` schema remains frozen. External integration must adapt to that contract rather than introducing runner-specific fields into v1.

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
