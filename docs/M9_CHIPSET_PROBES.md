# M9.3 chipset probes

M9.3 extends the frozen M9.2 CPU baseline into deterministic custom-chip evidence. Public CI remains ROM-free and uses only project-authored inputs.

## Sequence

1. **M9.3a — chipset pointer/register semantics.** Qualify OCS/ECS pointer masking and Copper list-pointer register writes before timing is introduced.
2. **M9.3b — Copper execution.** Execute a minimal project-authored Copper list and verify MOVE/WAIT/end behavior.
3. **M9.3c — Blitter.** Verify a bounded copy/minterm operation, completion state, zero flag, and DMA interaction.
4. **M9.3d — bitplanes and display DMA.** Verify representative fetch/address progression without relying on screenshots as the primary oracle.
5. **M9.3e — sprites.** Verify representative sprite DMA/register behavior.
6. **M9.3f — interrupts and DMA arbitration.** Verify custom-chip interrupt/DMA interactions at deterministic scheduler positions.

## M9.3a contract

The first probe deliberately tests register and address semantics rather than raster timing. It must establish that:

- OCS custom-chip pointers are masked to the OCS address domain and remain word aligned.
- ECS expands the pointer address domain while preserving word alignment.
- COP1LCH/COP1LCL and COP2LCH/COP2LCL compose addresses through the same chipset pointer rules.
- the result is emitted as `fellowng.compat-probe-result.v1` with subsystem `chipset`.

This small boundary gives later Copper/Blitter/DMA probes a qualified register foundation instead of mixing register bugs with timing bugs.


## M9.3a qualified baseline

M9.3a passed in GitHub Actions **M9 Compatibility Probes** run `36717262269` on commit `c0dd0558fd202e630682dd79f4b1e1d01f3abc23`.

Observed deterministic values:

- OCS masked pointer: `0x0002bcde`
- COP1LC after OCS high/low register writes: `0x0002bcde`
- ECS masked pointer: `0x001abcde`
- COP2LC after ECS high/low register writes: `0x001abcde`

The probe result validated as `fellowng.compat-probe-result.v1` with status `PASS`. M9.3a therefore freezes the register/pointer foundation for M9.3b Copper execution.
