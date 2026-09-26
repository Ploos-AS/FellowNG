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

### M8.2 — classic local qualification ✅

Classic local qualification is implemented through the common `tools/run_qualification_profile.py` runner and the three versioned reference profiles:

- `classic-a500-1x`: A500, 68000, OCS, 512 KiB chip RAM, Kickstart 1.x.
- `classic-a500plus-2x`: A500+, 68000, ECS, 1 MiB chip RAM, Kickstart 2.x.
- `classic-a1200-3x`: A1200, 68020, AGA, 2 MiB chip RAM plus 8 MiB fast RAM, Kickstart 3.x.

The runner requires an external asset directory containing `rom-path.txt`. Optional `workbench-path.txt`, `boot-adf-path.txt`, and `ext-path.txt` files may point to locally licensed Workbench/filesystem, boot floppy, and extended ROM assets. These path files and assets are local inputs and are not committed.

Example:

```sh
python3 tools/run_qualification_profile.py \
  profiles/qualification/classic-a500-1x.json \
  --fellowng ./build/fellowng-sdl \
  --assets /path/to/local/a500-assets \
  --evidence-dir evidence/classic-a500-1x
```

The same invocation pattern applies to the A500+ and A1200 profiles. A successful run must emit `fellowng.runtime-result.v1`, return exit status 0, match the profile's requested runtime mode, and produce a validated evidence bundle.

This completes the repository-side classic-local qualification support. Actual ROM/Workbench compatibility results are machine- and asset-specific and are recorded only when the licensed local assets are supplied. Public CI continues to use redistributable assets only.

### M8.3 — ARexx m68k qualification ✅

FellowNG now contains a project-authored guest probe at `probes/arexx/arexx-baseline-v1.rexx` plus host-side staging, execution-evidence, and parsing tools.

`tools/stage_arexx_probe.py` copies the probe into a user-supplied Workbench filesystem and records its SHA-256 plus the guest `RX` launch command. `tools/run_arexx_qualification.py` prepares the evidence directory and, after guest execution, gates the captured output through `tools/parse_arexx_probe.py`.

The parser emits the machine-readable `fellowng.arexx-result.v1` result and requires the versioned begin/pass markers, at least one test record, no failed test records, and no guest final-failure marker. This keeps the guest test result independent of screenshots and human log interpretation.

The qualification target is explicitly the emulated **m68k guest ARexx environment**. Host-side or x86 AROS ARexx execution is not accepted as a substitute. Licensed Workbench/ARexx components remain user-supplied and outside the repository.

This completes the repository-side deterministic ARexx m68k qualification path. Emulator/OS-specific compatibility evidence is produced when a licensed guest environment is supplied.

### M8.4 — runtime evidence contract

Standardize the evidence produced by qualification runs: runtime-result JSON, emulator/profile identity, logs, final framebuffer or screenshot when applicable, execution bounds, and exit status.

### M8.5 — configuration usability

Make portable configuration/profile selection and diagnostics convenient enough for normal interactive use while preserving the deterministic CLI path used by CI.

### M8.6 — release readiness

Produce versioned portable artifacts and release checks for supported hosts. Release work must retain the legacy Windows baseline until the portable frontend has an explicitly documented replacement policy.

### M8.7 — integration qualification ✅

External integration qualification passed in Ploos-AS/amiga-runtime GitHub Actions run #357 (2026-09-26), against amiga-runtime commit `7a91820069b7937ae8197a8d41920a48252267c8`.

The run built `fellowng-sdl` from source, detected the backend as available with automation contract `fellowng.runtime-result.v1`, and completed the redistributable `a1200-020-aros` sustained boot profile. FellowNG emitted `status=pass` after 16,384 pumps / 67,108,864 slices with observable framebuffer progress.

The emulator-neutral `qualify-aros-boot ... --emulator fellowng` path also passed and reported Q3, `guest_boot_proven=true`, and the FellowNG deterministic runtime-result as its proof. The four-backend comparison across FS-UAE, Amiberry, FellowNG, and Copperline completed with overall PASS, with all four backends reporting PASS for the same AROS m68k profile.

The same workflow's `regina-m68k` and runtime jobs both completed successfully, so this baseline is reproducible from public, redistributable inputs rather than copyrighted Kickstart/Workbench assets. This establishes the external integration baseline; classic proprietary-asset compatibility remains a local qualification concern.

### M8.8 — completion gate

M8 is complete when the production-integration baseline is reproducible, documented, externally consumable, and represented in an updated compatibility matrix.

## Constraints

- Do not store Kickstart ROMs or AmigaOS/Workbench media in Git.
- Preserve Fellow/WinFellow GPLv2 provenance and applicable upstream notices.
- Preserve Fellow's independent emulator lineage.
- Prefer deterministic, machine-readable qualification over screenshot-only success criteria.
- Keep public CI reproducible with legally redistributable assets.
