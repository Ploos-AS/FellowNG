#!/usr/bin/env python3
"""Stage FellowNG's project-authored ARexx probe into a local Workbench tree."""
from __future__ import annotations
import argparse, hashlib, json, pathlib, shutil

ap = argparse.ArgumentParser()
ap.add_argument("--workbench", required=True,
                help="path to a user-supplied extracted Workbench filesystem")
ap.add_argument("--probe", default="probes/arexx/arexx-baseline-v1.rexx")
ap.add_argument("--destination", default="T:FellowNG-ARexx-Probe.rexx",
                help="Amiga destination; currently T: or S: is supported")
ap.add_argument("--metadata")
args = ap.parse_args()

root = pathlib.Path(args.workbench).expanduser().resolve()
probe = pathlib.Path(args.probe).resolve()
if not root.is_dir():
    raise SystemExit(f"Workbench filesystem not found: {root}")
if not probe.is_file():
    raise SystemExit(f"ARexx probe not found: {probe}")

volume, sep, relative = args.destination.partition(":")
if not sep or volume.upper() not in ("T", "S") or not relative:
    raise SystemExit("destination must use T: or S:")
dest_dir = root / volume.upper()
dest_dir.mkdir(parents=True, exist_ok=True)
dest = dest_dir / relative
shutil.copyfile(probe, dest)

digest = hashlib.sha256(probe.read_bytes()).hexdigest()
metadata = {
    "schema": "fellowng.arexx-stage.v1",
    "probe": probe.name,
    "sha256": digest,
    "guest_path": args.destination,
    "launch": f"RX {args.destination}",
}
if args.metadata:
    pathlib.Path(args.metadata).write_text(json.dumps(metadata, indent=2, sort_keys=True) + "\n")

print(f"staged: {args.destination}")
print(f"sha256: {digest}")
print(f"guest launch: RX {args.destination}")
