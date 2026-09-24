#!/usr/bin/env python3
"""Self-tests for FellowNG release packaging/validation."""
from __future__ import annotations
import hashlib, pathlib, subprocess, sys, tarfile, tempfile, zipfile
ROOT=pathlib.Path(__file__).resolve().parents[1]; PY=sys.executable
V=ROOT/"tools/validate_release_archive.py"
def side(p):
 d=hashlib.sha256(p.read_bytes()).hexdigest(); pathlib.Path(str(p)+".sha256").write_text(f"{d}  {p.name}\n")
def run(p): return subprocess.run([PY,str(V),str(p)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode
with tempfile.TemporaryDirectory() as td:
 r=pathlib.Path(td); n="fellowng-1.0-linux-x86_64"; a=r/(n+".tar.gz")
 for x in ("LICENSE","README.md","fellowng-sdl"): (r/x).write_text("x")
 with tarfile.open(a,"w:gz") as t:
  for x in ("LICENSE","README.md","fellowng-sdl"): t.add(r/x,arcname=f"{n}/{x}")
 side(a); assert run(a)==0
 a.write_bytes(a.read_bytes()+b"x"); assert run(a)!=0
with tempfile.TemporaryDirectory() as td:
 r=pathlib.Path(td); n="fellowng-1.0-windows-x86_64"; a=r/(n+".zip")
 with zipfile.ZipFile(a,"w") as z:
  for x in ("LICENSE","README.md","fellowng-sdl.exe"): z.writestr(f"{n}/{x}","x")
  z.writestr(f"{n}/kick.rom","x")
 side(a); assert run(a)!=0
with tempfile.TemporaryDirectory() as td:
 r=pathlib.Path(td); n="fellowng-1.0-windows-x86_64"; a=r/(n+".zip")
 with zipfile.ZipFile(a,"w") as z:
  for x in ("LICENSE","README.md","fellowng-sdl.exe"): z.writestr(f"{n}/{x}","x")
  z.writestr(f"{n}/../evil.txt","x")
 side(a); assert run(a)!=0
print("release tooling tests PASS")
