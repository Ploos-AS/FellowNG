# M8.2 — Classic local qualification

Classic qualification uses user-supplied licensed Amiga assets. No Kickstart ROM,
Workbench/AmigaOS media, or paths to private assets are committed to FellowNG.

## Asset directory

Create a directory outside the repository for the selected profile. The runner
reads small text files containing absolute local paths:

- `rom-path.txt` — required Kickstart ROM
- `workbench-path.txt` — optional extracted Workbench filesystem
- `boot-adf-path.txt` — optional Workbench/boot ADF
- `ext-path.txt` — optional extended ROM

For the A500 1.x baseline, only `rom-path.txt` is required for the deterministic
ROM/runtime qualification. Add Workbench media when guest OS behavior is being
tested.

Example layout (paths are illustrative only):

```text
$HOME/.local/share/fellowng/qualification/a500-1x/
  rom-path.txt
  workbench-path.txt        # optional
  boot-adf-path.txt         # optional
```

## A500 / Kickstart 1.x

Build `fellowng-sdl`, then run:

```sh
python3 tools/run_qualification_profile.py \
  profiles/qualification/classic-a500-1x.json \
  --fellowng /path/to/fellowng-sdl \
  --assets "$HOME/.local/share/fellowng/qualification/a500-1x" \
  --evidence-dir ./evidence/classic-a500-1x
```

A successful run must emit a passing `fellowng.runtime-result.v1` result and
return exit status zero. The evidence directory contains `fellowng.log`,
`exit-status.txt`, `runtime-result.json`, and
`qualification-metadata.json`.

The evidence files contain runtime/profile results, not ROM or Workbench content.
Do not commit an evidence set if emulator output unexpectedly contains private
local paths or proprietary data.

## Remaining classic baselines

The same mechanism is used for:

- `classic-a500plus-2x.json`
- `classic-a1200-3x.json`

M8.2 is complete only after all three profiles have been exercised with
appropriate licensed local assets and their results have been recorded without
adding those assets to the repository.
