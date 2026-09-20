# M7 Compatibility matrix

This matrix records reproducible qualification evidence for FellowNG. It does not
claim cycle-exact equivalence with another emulator. Independent-emulator results
are behavioral reference observations.

## Public AROS qualification

| Profile | FellowNG | Independent reference | Evidence | Status |
| --- | --- | --- | --- | --- |
| AROS-m68k-020 | 68020, runtime/deep boot, runtime-result v1 | FS-UAE 3.1.66, 68020 | FellowNG JSON result; FS-UAE log + captured Xvfb frame | qualified |
| AROS-m68k-020-desktop | 68020, boot ADF + read-only Live CD filesystem, desktop framebuffer progress | FS-UAE baseline uses the same official AROS asset set; equivalent Live CD host-filesystem semantics are not yet asserted | FellowNG JSON result + framebuffer evidence | qualified in FellowNG; external desktop semantics not compared |

## Classic/reference profiles

| Profile | Public CI | Local qualification | Notes |
| --- | --- | --- | --- |
| A500-1.x | no | user-supplied licensed ROM/OS | OCS/68000 baseline |
| A500Plus-2.x | no | user-supplied licensed ROM/OS | ECS/68000 baseline |
| A1200-020 | no | user-supplied licensed ROM/OS | AGA/68020, 2 MiB Chip + 8 MiB Fast |

Copyrighted Kickstart/Workbench media is never committed to the repository.

## Known differences and limits

- FellowNG's public qualification uses its bounded runtime modes and
  `fellowng.runtime-result.v1`; FS-UAE does not expose the same result contract.
- The FS-UAE reference currently proves sustained external execution and
  observable display output using the same AROS boot asset family. It is not an
  instruction-by-instruction or cycle-timing comparison.
- Frame signatures are FellowNG-specific evidence and must not be compared
  numerically with screenshots produced by another emulator.
- Host filesystem integration differs between emulator implementations. The
  FellowNG AROS desktop profile therefore remains a FellowNG-specific end-to-end
  qualification until an equivalent external filesystem configuration is
  explicitly demonstrated.
- Timing-sensitive compatibility remains an area for targeted probes rather than
  inferred equivalence from successful boot.

## Reproduction

FellowNG public profiles are stored under `profiles/qualification/` and run via
`tools/run_qualification_profile.py`. AROS assets are obtained by
`tools/fetch_aros_m68k.py`.

The independent reference is implemented by
`.github/workflows/m7-fsuae-crosscheck.yml`. CI artifacts preserve the external
emulator log and captured display frame.

## M7 conclusion

The initial M7 scope establishes machine-readable public AROS profiles,
deterministic FellowNG qualification, and an independent FS-UAE behavioral
baseline while keeping classic proprietary-media profiles available for local
qualification. Future compatibility work can add narrower CPU, chipset, timing,
audio, input, storage, and application probes without changing the frozen M5
runtime-result v1 contract.
