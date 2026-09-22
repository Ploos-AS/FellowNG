# M8.3 — ARexx m68k qualification

M8.3 adds a deterministic guest-side ARexx probe for classic Amiga-compatible
m68k environments. Its purpose is to qualify ARexx behavior through FellowNG and
later compare the same probe through the other `amiga-runtime` emulator
backends.

## Scope and rule

The qualification target is ARexx executing inside the emulated m68k guest.
A host-side ARexx implementation, or an x86 AROS environment, is not a substitute
for this test.

The probe itself may be stored in FellowNG because it is project-authored text.
Kickstart, AmigaOS/Workbench and proprietary ARexx binaries/libraries must not be
stored in the repository.

## M8.3 sequence

1. **M8.3a — probe contract:** define deterministic inputs, output and exit/pass
   semantics.
2. **M8.3b — guest probe:** add a small portable `.rexx` script exercising the
   baseline language/runtime features.
3. **M8.3c — guest launch path:** stage and invoke the probe from a user-supplied
   m68k Workbench environment.
4. **M8.3d — evidence parser:** convert probe output into machine-readable
   qualification evidence without changing `fellowng.runtime-result.v1`.
5. **M8.3e — FellowNG local qualification:** run the probe on the supported
   classic profiles where ARexx is available.
6. **M8.3f — cross-emulator contract:** make the same guest probe consumable by
   `amiga-runtime` for FS-UAE and Amiberry comparison.

## Probe contract

The guest probe writes plain ASCII records so the evidence can be consumed
without interpreting screenshots:

```text
FELLOWNG_AREXX_PROBE_V1 BEGIN
TEST <id> PASS
TEST <id> FAIL <reason>
...
FELLOWNG_AREXX_PROBE_V1 PASS
```

A successful qualification requires the exact final `PASS` marker and no
`FAIL` record. Missing/truncated output is a failure, not an implicit pass.

The initial probe should cover deterministic language/runtime behavior only:
assignment and expression evaluation, string operations, numeric arithmetic,
conditional control flow, loops, procedures, and return/result handling.
Environment-specific GUI interaction, timing, locale, network access and
third-party ARexx ports are outside the baseline.

Later probes may test Amiga-specific ARexx ports separately, but they must not
weaken or replace this language/runtime baseline.

## Evidence

ARexx evidence is additive to the frozen `fellowng.runtime-result.v1` emulator
result. A qualification bundle should retain the emulator result plus raw probe
output and a separate parsed ARexx result. This avoids adding FellowNG-specific
guest-test fields to the frozen runtime schema.
