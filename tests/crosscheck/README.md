# Emulator cross-check records

Each record is JSON and follows fellowng.crosscheck.v1.

Example:

{
  "schema": "fellowng.crosscheck.v1",
  "profile": "aros-m68k-020",
  "probe": "sustained-boot",
  "emulator": {"name": "FS-UAE", "version": "external-record"},
  "status": "not-run",
  "host": {"os": "linux", "arch": "unknown"},
  "notes": "Run with the exact AROS asset revision used by the FellowNG qualification."
}

Store only metadata and observed results. Never commit copyrighted ROMs, OS images, or other proprietary Amiga media.