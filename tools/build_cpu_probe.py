#!/usr/bin/env python3
"""Build the M9 bare-metal 68k probe with a GNU m68k cross toolchain."""
import argparse
import pathlib
import shutil
import subprocess
import tempfile

ap = argparse.ArgumentParser()
ap.add_argument("source")
ap.add_argument("--output", required=True)
args = ap.parse_args()

as_cmd = shutil.which("m68k-linux-gnu-as")
objcopy = shutil.which("m68k-linux-gnu-objcopy")
ld = shutil.which("m68k-linux-gnu-ld")
if not all((as_cmd, objcopy, ld)):
    raise SystemExit("m68k GNU binutils required (m68k-linux-gnu-as/ld/objcopy)")

with tempfile.TemporaryDirectory() as td:
    o = pathlib.Path(td) / "probe.o"
    elf = pathlib.Path(td) / "probe.elf"
    linker_script = pathlib.Path(td) / "probe.ld"

    linker_script.write_text(
        """SECTIONS
{
  . = 0;
  .vectors : { *(.vectors) }
  . = 0x400;
  .text : { *(.text*) }
  . = 0x1000;
  .data : { *(.data*) }
}
"""
    )

    subprocess.run([as_cmd, "-m68000", "-o", str(o), args.source], check=True)
    subprocess.run([ld, "-T", str(linker_script), "-o", str(elf), str(o)], check=True)
    subprocess.run([objcopy, "-O", "binary", str(elf), args.output], check=True)

print("built:", args.output)
