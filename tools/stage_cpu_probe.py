#!/usr/bin/env python3
"""Validate and stage a project-authored bare-metal m68k CPU probe image."""
import argparse, hashlib, json
from pathlib import Path

ap=argparse.ArgumentParser()
ap.add_argument("manifest")
ap.add_argument("--image")
ap.add_argument("--output")
args=ap.parse_args()
m=json.loads(Path(args.manifest).read_text())
if m.get("schema")!="fellowng.cpu-probe-image.v1":
    raise SystemExit("unsupported CPU probe manifest schema")
for k in ("id","cpu","subsystem","result_schema"):
    if not isinstance(m.get(k),str) or not m[k]:
        raise SystemExit(f"invalid CPU probe manifest field: {k}")
if m["cpu"] not in ("68000","68020"):
    raise SystemExit("unsupported CPU probe model")
if m["result_schema"]!="fellowng.compat-probe-result.v1":
    raise SystemExit("unexpected result schema")
if not isinstance(m.get("cases"),list) or not m["cases"]:
    raise SystemExit("CPU probe must declare cases")
if args.image:
    p=Path(args.image)
    if not p.is_file(): raise SystemExit("CPU probe image not found")
    digest=hashlib.sha256(p.read_bytes()).hexdigest()
    expected=m.get("sha256")
    if expected and digest!=expected: raise SystemExit("CPU probe SHA-256 mismatch")
    if args.output:
        Path(args.output).write_bytes(p.read_bytes())
    print(f"image-sha256: {digest}")
print(f"cpu probe manifest PASS: {m['id']} ({m['cpu']}, {len(m['cases'])} cases)")
