#!/usr/bin/env python3
"""Validate a FellowNG qualification evidence bundle."""
from __future__ import annotations
import argparse, hashlib, json, pathlib

ap = argparse.ArgumentParser()
ap.add_argument("evidence_dir")
args = ap.parse_args()

root = pathlib.Path(args.evidence_dir)
manifest_path = root / "manifest.json"
if not manifest_path.is_file():
    raise SystemExit("missing manifest.json")
manifest = json.loads(manifest_path.read_text())
if manifest.get("schema") != "fellowng.evidence-manifest.v1":
    raise SystemExit("unsupported evidence manifest schema")

files = manifest.get("files")
if not isinstance(files, dict):
    raise SystemExit("manifest files must be an object")

required = ("runtime-result.json", "qualification-metadata.json",
            "exit-status.txt", "fellowng.log")
for name in required:
    entry = files.get(name)
    if not isinstance(entry, dict) or entry.get("available") is not True:
        raise SystemExit(f"required evidence unavailable: {name}")

for name, entry in files.items():
    if not isinstance(entry, dict):
        raise SystemExit(f"invalid manifest entry: {name}")
    if entry.get("available") is not True:
        continue
    if pathlib.PurePosixPath(name).is_absolute() or ".." in pathlib.PurePosixPath(name).parts:
        raise SystemExit(f"unsafe evidence path: {name}")
    path = root / name
    if not path.is_file():
        raise SystemExit(f"manifested evidence missing: {name}")
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != entry.get("sha256"):
        raise SystemExit(f"SHA-256 mismatch: {name}")
    if len(data) != entry.get("size"):
        raise SystemExit(f"size mismatch: {name}")

runtime = json.loads((root / "runtime-result.json").read_text())
if runtime.get("schema") != "fellowng.runtime-result.v1":
    raise SystemExit("unsupported runtime result schema")
if runtime.get("status") != "pass":
    raise SystemExit("runtime result is not PASS")

try:
    exit_status = int((root / "exit-status.txt").read_text().strip())
except ValueError as exc:
    raise SystemExit("invalid exit-status.txt") from exc
if exit_status != 0:
    raise SystemExit(f"runtime exit status is not zero: {exit_status}")

arexx = files.get("arexx-result.json", {})
if arexx.get("available") is True:
    result = json.loads((root / "arexx-result.json").read_text())
    if result.get("schema") != "fellowng.arexx-result.v1":
        raise SystemExit("unsupported ARexx result schema")
    if result.get("status") != "pass":
        raise SystemExit("ARexx result is not PASS")

print("evidence bundle PASS")
