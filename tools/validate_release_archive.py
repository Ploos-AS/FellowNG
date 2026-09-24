#!/usr/bin/env python3
"""Validate a FellowNG release archive and its SHA-256 sidecar."""
from __future__ import annotations
import argparse, hashlib, pathlib, re, tarfile, zipfile

ap=argparse.ArgumentParser()
ap.add_argument("archive")
ap.add_argument("--checksum")
args=ap.parse_args()
archive=pathlib.Path(args.archive)
if not archive.is_file(): raise SystemExit(f"archive not found: {archive}")
side=pathlib.Path(args.checksum) if args.checksum else pathlib.Path(str(archive)+".sha256")
if not side.is_file(): raise SystemExit(f"checksum not found: {side}")
m=re.fullmatch(r"([0-9a-fA-F]{64})  (.+)\n?",side.read_text())
if not m or m.group(2)!=archive.name: raise SystemExit("invalid checksum sidecar")
if hashlib.sha256(archive.read_bytes()).hexdigest().lower()!=m.group(1).lower(): raise SystemExit("checksum mismatch")
base=archive.name[:-4] if archive.name.endswith(".zip") else archive.name[:-7] if archive.name.endswith(".tar.gz") else None
if not base or not re.fullmatch(r"fellowng-.+-(linux|macos|windows)-(x86_64|arm64)",base): raise SystemExit("invalid release archive name")
if zipfile.is_zipfile(archive):
    names=zipfile.ZipFile(archive).namelist()
elif tarfile.is_tarfile(archive):
    with tarfile.open(archive,"r:*") as t: names=t.getnames()
else: raise SystemExit("unsupported archive format")
files=[]
for raw in names:
    p=pathlib.PurePosixPath(raw)
    if p.is_absolute() or ".." in p.parts: raise SystemExit(f"unsafe archive path: {raw}")
    if not p.parts or p.parts[0]!=base: raise SystemExit(f"unexpected archive root: {raw}")
    if raw.endswith("/"): continue
    files.append(p)
required={"LICENSE","README.md"}
leaf={p.name for p in files}
if not required.issubset(leaf): raise SystemExit("required release files missing")
banned={".rom",".adf",".hdf",".ipf",".dms",".lha"}
for p in files:
    if p.suffix.lower() in banned: raise SystemExit(f"proprietary/guest asset forbidden: {p}")
allowed={"LICENSE","README.md","fellowng-sdl","fellowng-sdl.exe","SDL3.dll"}
unexpected=leaf-allowed
if unexpected: raise SystemExit("unexpected release content: "+", ".join(sorted(unexpected)))
if not ({"fellowng-sdl","fellowng-sdl.exe"} & leaf): raise SystemExit("FellowNG executable missing")
print(f"release archive validation PASS: {archive.name}")
