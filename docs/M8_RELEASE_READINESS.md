# M8.6 — Release readiness

FellowNG releases must turn the already-qualified portable builds into
versioned, inspectable artifacts without weakening the existing host matrix.

## M8.6a — Release artifact contract

Initial portable release targets:

| Host | Architecture | Frontend | Artifact identity |
| --- | --- | --- | --- |
| Linux | x86-64 | SDL3 | fellowng-<version>-linux-x86_64 |
| Linux | arm64 | SDL3 | fellowng-<version>-linux-arm64 |
| macOS | arm64 | SDL3 | fellowng-<version>-macos-arm64 |
| Windows | x86-64 | SDL3 | fellowng-<version>-windows-x86_64 |

The legacy Windows/MSBuild build remains a regression requirement but is not
implicitly promoted to the portable artifact contract.

Every release artifact must contain the FellowNG executable(s), GPLv2 license
and required project/upstream attribution. Redistributable runtime libraries may
be bundled when needed by the host packaging method.

Kickstart ROMs, Workbench/AmigaOS files and other proprietary guest assets must
never be included.

## Version identity

Release automation receives an explicit version derived from the release tag.
Artifact filenames and `--version` output must be traceable to that release
identity. Development builds may retain a development revision identifier.

## Release gate

Before publication, the corresponding host build must pass:

1. compile/build;
2. `--help` and `--version`;
3. portable frontend self-test;
4. applicable repository/CTest checks;
5. artifact content check;
6. SHA-256 generation.

Public AROS qualification remains separate runtime evidence and does not permit
AROS or AmigaOS assets to be bundled into FellowNG release archives.

## Planned sequence

- M8.6a: release artifact/version contract.
- M8.6b: version injection and release identity.
- M8.6c: host packaging scripts.
- M8.6d: release-artifact CI matrix.
- M8.6e: checksums and release validation.
- M8.6f: qualified release workflow.
