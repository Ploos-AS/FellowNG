#!/usr/bin/env python3
"""Fetch the current official AROS amiga-m68k boot ISO and extract its ROMs."""
from __future__ import annotations
import html, pathlib, re, subprocess, sys, urllib.parse, urllib.request

INDEX = "https://aros.sourceforge.io/cgi-bin/files?lang=en&type=nightly2"
out = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else "build-m4/aros-m68k")
out.mkdir(parents=True, exist_ok=True)

page = urllib.request.urlopen(INDEX, timeout=60).read().decode("utf-8", "replace")
links = [html.unescape(x) for x in re.findall(r'href=["\']([^"\']+)["\']', page, re.I)]
candidates = [urllib.parse.urljoin(INDEX, x) for x in links if "amiga-m68k-boot-iso" in x.lower()]
if not candidates:
    raise SystemExit("official AROS nightly page did not expose amiga-m68k-boot-iso")
url = candidates[0]
archive = out / pathlib.Path(urllib.parse.urlparse(url).path).name
print(f"AROS source: {url}")
urllib.request.urlretrieve(url, archive)

subprocess.run(["7z", "x", "-y", f"-o{out}", str(archive)], check=True)
isos = list(out.rglob("*.iso"))
for iso in isos:
    subprocess.run(["7z", "x", "-y", f"-o{out / 'iso'}", str(iso)], check=True)

names = ("aros-rom.bin", "aros-amiga-m68k-rom.bin", "aros.rom")
extnames = ("aros-ext.bin", "aros-amiga-m68k-ext.bin")
files = [p for p in out.rglob("*") if p.is_file()]
rom = next((p for p in files if p.name.lower() in names), None)
ext = next((p for p in files if p.name.lower() in extnames), None)
if rom is None:
    raise SystemExit("AROS m68k main ROM not found after extraction")
(out / "rom-path.txt").write_text(str(rom.resolve()) + "\n")
(out / "ext-path.txt").write_text((str(ext.resolve()) if ext else "") + "\n")

# The boot ISO also carries bootable Amiga media.  Prefer an ADF that can be
# attached directly to Fellow as DF0; keep discovery deterministic and fail
# loudly if the official nightly layout stops exposing one.
adfs = sorted((p for p in files if p.suffix.lower() == ".adf"), key=lambda p: (len(str(p)), str(p).lower()))
boot_adf = next((p for p in adfs if "boot" in p.name.lower()), adfs[0] if adfs else None)
(out / "boot-adf-path.txt").write_text((str(boot_adf.resolve()) if boot_adf else "") + "\n")

print(f"AROS main ROM: {rom}")
print(f"AROS extended ROM: {ext if ext else 'not present'}")
print(f"AROS boot ADF: {boot_adf if boot_adf else 'not present'}")
