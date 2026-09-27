# M9.2 CPU probe images

M9 CPU qualification uses small project-authored m68k guest images. They are intentionally independent of Kickstart and Workbench so CPU/exception evidence does not depend on proprietary operating-system assets.

## Image contract

A CPU probe image is a raw big-endian m68k payload plus a JSON manifest using `fellowng.cpu-probe-image.v1`. The manifest records CPU model, load/reset addresses, expected probe-result identity, and the compatibility subsystem.

The first target, `m68000-exception-vectors-v1`, exercises real guest-side 68000 exception/vector/stack-frame behavior. Planned cases are illegal instruction, integer divide by zero, TRAP, privilege violation, and address-error/alignment handling. The probe must report observations through a deterministic guest-to-host evidence channel before M9.2a can be marked complete.

A later 68020 payload covers VBR and 68020 stack behavior separately.

## Integrity and licensing

Probe source and generated payloads must be project-authored or redistributable. No Kickstart, Workbench, AmigaOS, commercial software, or extracted proprietary code may be used.

The host staging tool validates manifest shape and SHA-256 when a payload digest is present. Staging alone is not compatibility evidence: PASS requires execution by FellowNG and a valid `fellowng.compat-probe-result.v1` result.
