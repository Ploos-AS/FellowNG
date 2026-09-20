#!/usr/bin/env python3
"""Run a FellowNG AROS qualification profile."""
from __future__ import annotations
import argparse, json, os, pathlib, subprocess, sys

def read_path(root: pathlib.Path, name: str, required: bool = True) -> str:
    p = root / name
    value = p.read_text().strip() if p.exists() else ""
    if required and not value:
        raise SystemExit(f"required AROS asset missing: {name}")
    return value

ap = argparse.ArgumentParser()
ap.add_argument("profile")
ap.add_argument("--fellowng", required=True)
ap.add_argument("--assets", required=True)
ap.add_argument("--evidence-dir", default=".")
args = ap.parse_args()

profile = json.loads(pathlib.Path(args.profile).read_text())
if profile.get("schema") != "fellowng.qualification-profile.v1":
    raise SystemExit("unsupported qualification profile schema")
if profile.get("family") != "aros-ci":
    raise SystemExit("profile is not an AROS CI profile")

assets = pathlib.Path(args.assets)
q = profile["qualification"]
cmd = [args.fellowng, "--" + q["mode"], "--result-json",
       "--max-pumps", str(q["max_pumps"]), "-s",
       "kickstart_rom_file=" + read_path(assets, "rom-path.txt"), "-s",
       "cpu_type=" + profile["machine"]["cpu"]]

ext = read_path(assets, "ext-path.txt", False)
if ext:
    cmd += ["-s", "kickstart_rom_file_ext=" + ext]

if q["mode"] == "runtime-boot-desktop":
    adf = read_path(assets, "boot-adf-path.txt")
    live = read_path(assets, "live-root-path.txt")
    fs = profile["filesystem"]
    cmd += ["-s", "floppy0=" + adf, "-s", "floppy0_enabled=yes",
            "-s", "floppy0_readonly=yes", "-s", "autoconfig=yes",
            "-s", f"filesystem=ro,{fs['live_volume']}:{live}"]

env = os.environ.copy()
env.setdefault("SDL_VIDEODRIVER", "dummy")
env.setdefault("SDL_AUDIODRIVER", "dummy")
print("profile:", profile["id"], "revision", profile["revision"])
proc = subprocess.run(cmd, env=env, text=True, stdout=subprocess.PIPE,
                      stderr=subprocess.STDOUT)
print(proc.stdout, end="")
lines = proc.stdout.splitlines()
results = [json.loads(x) for x in lines if x.startswith('{"schema":')]
if not results:
    raise SystemExit("FellowNG emitted no runtime result")
result = results[-1]
if result.get("schema") != q["result_schema"]:
    raise SystemExit("unexpected runtime result schema")
if result.get("status") != "pass" or proc.returncode != 0:
    raise SystemExit(proc.returncode or 1)
if result.get("mode") != q["mode"]:
    raise SystemExit("runtime result mode does not match profile")
print(f"qualification profile PASS: {profile['id']}")
