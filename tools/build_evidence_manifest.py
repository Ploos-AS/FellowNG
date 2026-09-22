#!/usr/bin/env python3
"""Build a portable FellowNG qualification evidence manifest."""
from __future__ import annotations
import argparse, hashlib, json, pathlib, platform

ap = argparse.ArgumentParser()
ap.add_argument("evidence_dir")
ap.add_argument("--profile")
ap.add_argument("--profile-revision", type=int)
ap.add_argument("--fellowng-revision")
ap.add_argument("--mode")
ap.add_argument("--max-pumps", type=int)
args = ap.parse_args()

root = pathlib.Path(args.evidence_dir)
if not root.is_dir():
    raise SystemExit(f"evidence directory not found: {root}")

names = [
    "runtime-result.json", "qualification-metadata.json", "exit-status.txt",
    "fellowng.log", "arexx-stage.json", "arexx-guest-output.txt",
    "arexx-result.json",
]
# Include framebuffer evidence without assuming one image format.
names += sorted(p.name for p in root.glob("framebuffer.*") if p.is_file())

files = {}
for name in dict.fromkeys(names):
    p = root / name
    if p.is_file():
        files[name] = {
            "available": True,
            "sha256": hashlib.sha256(p.read_bytes()).hexdigest(),
            "size": p.stat().st_size,
        }
    else:
        files[name] = {"available": False}

manifest = {
    "schema": "fellowng.evidence-manifest.v1",
    "fellowng_revision": args.fellowng_revision,
    "profile": args.profile,
    "profile_revision": args.profile_revision,
    "host": {"os": platform.system(), "architecture": platform.machine()},
    "runtime": {"mode": args.mode, "max_pumps": args.max_pumps},
    "files": files,
}
out = root / "manifest.json"
out.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")
print(out)
