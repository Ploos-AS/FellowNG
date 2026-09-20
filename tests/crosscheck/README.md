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
## CI evidence

The M7.3 GitHub Actions job records a native FS-UAE run using the same official AROS m68k boot assets as the FellowNG qualification. The workflow captures a virtual-display frame after the boot grace period and preserves the emulator log as artifacts. A green job establishes that the external emulator started and produced observable display output; it does not by itself establish semantic equivalence with FellowNG.
