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


## M8.3f — Cross-emulator contract

The same guest probe and parser are the interoperability boundary for FellowNG,
FS-UAE and Amiberry. An external orchestrator such as `amiga-runtime` must not
rewrite the probe per emulator.

Each backend is responsible only for:

1. presenting the staged `arexx-baseline-v1.rexx` file to the same licensed
   m68k Workbench guest;
2. invoking `RX T:FellowNG-ARexx-Probe.rexx` (or an equivalent staged guest
   path recorded in `fellowng.arexx-stage.v1`);
3. capturing the probe's plain-ASCII output without altering its records;
4. preserving emulator/runtime evidence independently from guest ARexx evidence;
5. parsing the captured output with `tools/parse_arexx_probe.py`.

A comparable result tuple is:

```text
emulator
emulator_version
machine_profile
guest_os_profile
probe_sha256
fellowng.arexx-result.v1
emulator_runtime_result
```

`probe_sha256` must match across compared runs. A PASS from one emulator does
not imply PASS for another; every backend records its own observed result.

The external runner may wrap these fields in its own versioned result envelope,
but must preserve the raw `fellowng.arexx-result.v1` document and must not add
backend-specific semantics to the guest probe.

### Qualification state

| Backend | Contract | Local m68k execution |
| --- | --- | --- |
| FellowNG | READY | NOT RUN |
| FS-UAE | READY for amiga-runtime adapter | NOT RUN |
| Amiberry | READY for amiga-runtime adapter | NOT RUN |

These rows become PASS only after the identical probe has actually executed in
the corresponding m68k guest and produced passing evidence.
