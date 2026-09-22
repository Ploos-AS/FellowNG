# M8.4 — Runtime evidence contract

This contract defines the portable evidence bundle produced by FellowNG
qualification and consumed by external orchestration such as `amiga-runtime`.
It does not replace the frozen `fellowng.runtime-result.v1` runtime result.

## Bundle layout

A qualification run should archive:

```text
evidence/
  manifest.json
  runtime-result.json
  qualification-metadata.json
  exit-status.txt
  fellowng.log
  framebuffer.*                 # when requested/available
  arexx-stage.json              # ARexx qualification only
  arexx-guest-output.txt        # ARexx qualification only
  arexx-result.json             # ARexx qualification only
```

Only applicable files are required. Missing optional evidence must be represented
in the manifest rather than replaced with fabricated placeholders.

## Manifest

`manifest.json` uses schema `fellowng.evidence-manifest.v1` and records:

- FellowNG revision/version when known;
- qualification profile identifier and revision;
- host OS and architecture;
- runtime mode and deterministic pump bound;
- process exit status;
- relative evidence filenames;
- SHA-256 digests for archived evidence;
- explicit availability for optional framebuffer and guest-probe evidence.

Paths in the manifest are relative to the evidence directory. Private absolute
host paths must not be archived.

## Pass semantics

A runtime PASS requires both process exit status zero and a passing
`fellowng.runtime-result.v1` document. Guest probes such as ARexx add their own
result contract; a guest-probe qualification is PASS only when both the emulator
runtime gate and the guest probe gate pass.

A screenshot or framebuffer is supporting evidence and never substitutes for a
machine-readable result.

## Asset boundary

Kickstart ROMs, Workbench/AmigaOS files, disk images and other proprietary assets
are inputs, never evidence. The bundle must not copy them. Logs and metadata
should avoid private absolute paths; external runners should redact or normalize
such paths before publishing evidence.

## Cross-emulator use

External runners may wrap this bundle in their own versioned schema. They should
preserve FellowNG's raw runtime result and guest result documents, profile
identity, execution bounds, exit status and evidence digests so observations can
be compared with FS-UAE and Amiberry without pretending their internal runtime
contracts are identical.
