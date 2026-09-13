# Upstream provenance

FellowNG is based on the Fellow/WinFellow Amiga emulator source tree.

## Upstream

Primary upstream project:

- WinFellow: https://github.com/petschau/WinFellow

FellowNG is intended as a modern, cross-platform continuation and portability effort. It is not a clean-room emulator and does not claim ownership of the original Fellow/WinFellow work.

The imported source tree remains subject to its existing copyright notices and license terms. Contributors must preserve relevant notices when moving or modifying code.

## License

The repository is distributed under GNU GPL version 2 as inherited from WinFellow. Components with their own compatible notices retain those notices.

## Relationship to UAE

Fellow has its own emulator lineage. The WinFellow tree also contains a filesystem module derived from WinUAE and supporting UAE-derived files, as documented by upstream. FellowNG will preserve that provenance explicitly.

A major reason for continuing Fellow is to retain an emulator implementation that is substantially independent from the UAE family, which is useful both for users and for cross-validation of Amiga software behavior.

## FellowNG policy

- Preserve upstream attribution.
- Keep upstream-derived history and notices whenever practical.
- Prefer clearly separated portability changes.
- Document substantial imports from other projects before merging them.
- Do not introduce ROM images, Workbench media, or other proprietary Amiga software into the repository.
