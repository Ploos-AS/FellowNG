#!/usr/bin/env python3
"""Create a portable FellowNG release archive without guest assets."""
from __future__ import annotations
import argparse, hashlib, pathlib, platform, shutil, tarfile, zipfile

ap = argparse.ArgumentParser()
ap.add_argument("--version", required=True)
ap.add_argument("--binary", required=True)
ap.add_argument("--output-dir", default="dist")
ap.add_argument("--host", choices=("linux", "macos", "windows"))
ap.add_argument("--arch", choices=("x86_64", "arm64"))
args = ap.parse_args()

binary = pathlib.Path(args.binary)
if not binary.is_file():
    raise SystemExit(f"binary not found: {binary}")
host = args.host or {"Linux":"linux","Darwin":"macos","Windows":"windows"}.get(platform.system())
arch = args.arch or {"x86_64":"x86_64","AMD64":"x86_64","arm64":"arm64","aarch64":"arm64"}.get(platform.machine())
if not host or not arch:
    raise SystemExit("unsupported host/architecture; pass --host and --arch explicitly")

name = f"fellowng-{args.version}-{host}-{arch}"
out = pathlib.Path(args.output_dir)
stage = out / name
if stage.exists():
    shutil.rmtree(stage)
stage.mkdir(parents=True)
shutil.copy2(binary, stage / binary.name)
for required in ("LICENSE", "README.md"):
    src = pathlib.Path(required)
    if not src.is_file():
        raise SystemExit(f"required release file missing: {required}")
    shutil.copy2(src, stage / required)

# Deliberately package only the allow-listed files above. Guest ROM/OS/disk assets
# can therefore never enter the archive merely because they exist in a build tree.
if host == "windows":
    archive = out / f"{name}.zip"
    with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED) as z:
        for p in sorted(stage.iterdir()):
            z.write(p, f"{name}/{p.name}")
else:
    archive = out / f"{name}.tar.gz"
    with tarfile.open(archive, "w:gz") as t:
        t.add(stage, arcname=name)

digest = hashlib.sha256(archive.read_bytes()).hexdigest()
(out / f"{archive.name}.sha256").write_text(f"{digest}  {archive.name}\n")
print(archive)
print(f"sha256 {digest}")
