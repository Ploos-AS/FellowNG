# M8.5 — Configuration usability

FellowNG keeps deterministic CLI automation stable while making portable
configuration easier to inspect and diagnose.

## M8.5a — Layering contract

Configuration is resolved in this order:

1. emulator defaults;
2. optional user/profile configuration file;
3. explicit command-line Fellow options.

Later layers override earlier layers. Automation runners should continue to pass
all qualification-critical values explicitly; interactive users may rely on a
configuration file for convenience.

## Portable configuration goals

- accept a human-readable configuration file without changing
  `fellowng.runtime-result.v1`;
- provide a diagnostic mode that prints the resolved configuration and its
  provenance without starting emulation;
- reject unknown or malformed portable configuration keys with actionable
  diagnostics;
- keep host paths out of published machine-readable qualification evidence;
- never auto-discover proprietary Kickstart/Workbench assets in deterministic
  qualification mode;
- preserve ordinary Fellow option compatibility rather than inventing a second
  emulator configuration language unnecessarily.

## Proposed CLI surface

```text
fellowng-sdl --config <file> [frontend options] [Fellow options]
fellowng-sdl --config <file> --print-config
```

`--config` loads a portable configuration source. `--print-config` prints
the resolved values and their source (default, config file, command line) and
exits without booting the guest.

The implementation should reuse Fellow's existing option semantics wherever
possible. M8.5 will add only the frontend-side loading/provenance layer needed
for portability and diagnostics.

## Milestones

- M8.5a: configuration layering and diagnostics contract.
- M8.5b: portable `--config` loading.
- M8.5c: `--print-config` resolved configuration diagnostics.
- M8.5d: malformed/unknown-option diagnostics and regression tests.
- M8.5e: interactive configuration documentation and CI qualification.


## M8.5e — Interactive use and qualified baseline

The portable configuration path is now qualified in CI.

Example:

```text
# fellowng.conf
cpu_type=68020
chipmem_size=4
```

Inspect it without booting:

```sh
fellowng-sdl --config fellowng.conf --print-config
```

Override a value explicitly:

```sh
fellowng-sdl --config fellowng.conf --print-config -s cpu_type=68000
```

The printed provenance makes the layering visible. Explicit command-line Fellow
options remain later inputs than file-backed options and therefore retain the
intended override position.

The CI baseline covers comments, plain `key=value`, existing `-s key=value`
syntax, provenance output, command-line override input, missing files, and
malformed lines. The Linux SDL3 workflow passes this regression suite.

Qualification automation should still specify all boot-critical values
explicitly. `--config` is primarily an interactive/integration convenience and
does not authorize automatic discovery of proprietary assets.
