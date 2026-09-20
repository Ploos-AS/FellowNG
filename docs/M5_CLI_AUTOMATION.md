# FellowNG CLI automation

FellowNG's SDL3 frontend exposes a bounded command-line interface intended for CI,
scripted qualification, and external runners such as `amiga-runtime`.

## Discovery

```sh
fellowng-sdl --help
fellowng-sdl --version
```

## Boot modes

```text
--runtime-boot          bounded boot qualification
--runtime-boot-deep     deeper boot qualification
--runtime-boot-desktop  boot until qualified visible desktop progress
```

Normal Fellow configuration arguments may follow the frontend mode. Frontend-only
automation options are consumed by the SDL frontend and are not forwarded to the
legacy Fellow configuration parser.

## Machine-readable results

Add `--result-json` to a boot mode. Successful runs emit one JSON object using
the stable schema identifier:

```text
fellowng.runtime-result.v1
```

Consumers should use `status` to distinguish `pass` and `fail`, treat the
process exit status as authoritative, and ignore human-readable diagnostic output
when parsing automation results. The formal contract is
`docs/schema/runtime-result-v1.schema.json`.

A successful result includes the boot `mode`, execution counters, framebuffer
progress counters, the final frame signature, and visible-pixel count. A failed
qualified boot includes `status=fail` and a machine-readable `reason`.

## Deterministic execution bound

```sh
fellowng-sdl --runtime-boot-desktop --result-json --max-pumps 16384 [Fellow options]
```

`--max-pumps N` bounds frontend pump iterations. It is intended for callers that
must guarantee finite execution independently of guest behavior. A caller may
also impose an outer process timeout as a crash/hang guard; that timeout should
not replace FellowNG's own bounded qualification.

## Runner integration

An external runner should:

1. select one boot mode;
2. request `--result-json`;
3. set an explicit `--max-pumps` appropriate to the qualification profile;
4. supply ROM, filesystem, and machine configuration through normal Fellow options;
5. capture stdout/stderr and the process exit status;
6. parse the final line beginning with the runtime schema object;
7. validate the object against `runtime-result-v1.schema.json`;
8. archive framebuffer evidence when desktop qualification is requested.

ROMs and Amiga OS media remain external assets. Public automation must use
redistributable assets such as the AROS m68k ROMs used by FellowNG CI, or
user-supplied licensed ROM/media.

## Contract stability

The `fellowng.runtime-result.v1` identifier is the compatibility boundary for
external automation. Incompatible result-shape changes require a new schema
version rather than silently changing v1.
