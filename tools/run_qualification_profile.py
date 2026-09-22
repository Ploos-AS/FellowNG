#!/usr/bin/env python3
"""Run a FellowNG qualification profile."""
from __future__ import annotations
import argparse, json, os, pathlib, subprocess

def read_path(root: pathlib.Path, name: str, required: bool = True) -> str:
    p = root / name
    value = p.read_text().strip() if p.exists() else ""
    if required and not value:
        raise SystemExit(f"required qualification asset missing: {name}")
    if value and not pathlib.Path(value).exists():
        raise SystemExit(f"qualification asset path does not exist ({name}): {value}")
    return value

ap = argparse.ArgumentParser()
ap.add_argument("profile")
ap.add_argument("--fellowng", required=True)
ap.add_argument("--assets", required=True,
                help="directory containing asset path files; assets themselves remain outside the repository")
ap.add_argument("--evidence-dir", default=".")
args = ap.parse_args()

profile = json.loads(pathlib.Path(args.profile).read_text())
if profile.get("schema") != "fellowng.qualification-profile.v1":
    raise SystemExit("unsupported qualification profile schema")
family = profile.get("family")
if family not in ("aros-ci", "classic-local"):
    raise SystemExit(f"unsupported qualification profile family: {family}")

assets = pathlib.Path(args.assets)
q = profile["qualification"]
cmd = [args.fellowng, "--" + q["mode"], "--result-json",
       "--max-pumps", str(q["max_pumps"]), "-s",
       "kickstart_rom_file=" + read_path(assets, "rom-path.txt"), "-s",
       "cpu_type=" + profile["machine"]["cpu"]]

ext = read_path(assets, "ext-path.txt", False)
if ext:
    cmd += ["-s", "kickstart_rom_file_ext=" + ext]

if family == "aros-ci" and q["mode"] == "runtime-boot-desktop":
    adf = read_path(assets, "boot-adf-path.txt")
    live = read_path(assets, "live-root-path.txt")
    fs = profile["filesystem"]
    cmd += ["-s", "floppy0=" + adf, "-s", "floppy0_enabled=yes",
            "-s", "floppy0_readonly=yes", "-s", "autoconfig=yes",
            "-s", f"filesystem=ro,{fs['live_volume']}:{live}"]

if family == "classic-local":
    # Optional licensed boot media/filesystem. Path files contain only local
    # references; copyrighted assets must never be copied into the repository.
    workbench = read_path(assets, "workbench-path.txt", False)
    boot_adf = read_path(assets, "boot-adf-path.txt", False)
    if workbench:
        cmd += ["-s", "autoconfig=yes", "-s", f"filesystem=rw,Workbench:{workbench}"]
    if boot_adf:
        cmd += ["-s", "floppy0=" + boot_adf, "-s", "floppy0_enabled=yes",
                "-s", "floppy0_readonly=yes"]

env = os.environ.copy()
env.setdefault("SDL_VIDEODRIVER", "dummy")
env.setdefault("SDL_AUDIODRIVER", "dummy")
evidence_dir = pathlib.Path(args.evidence_dir)
evidence_dir.mkdir(parents=True, exist_ok=True)
print("profile:", profile["id"], "revision", profile["revision"], "family", family)
proc = subprocess.run(cmd, env=env, text=True, stdout=subprocess.PIPE,
                      stderr=subprocess.STDOUT)
print(proc.stdout, end="")
(evidence_dir / "fellowng.log").write_text(proc.stdout)
(evidence_dir / "exit-status.txt").write_text(str(proc.returncode) + "\n")
lines = proc.stdout.splitlines()
results = [json.loads(x) for x in lines if x.startswith('{"schema":')]
if not results:
    raise SystemExit("FellowNG emitted no runtime result")
result = results[-1]
(evidence_dir / "runtime-result.json").write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")
metadata = {
    "schema": "fellowng.qualification-evidence.v1",
    "profile": profile["id"],
    "profile_revision": profile["revision"],
    "family": family,
    "mode": q["mode"],
    "max_pumps": q["max_pumps"],
    "cpu": profile["machine"]["cpu"],
}
(evidence_dir / "qualification-metadata.json").write_text(
    json.dumps(metadata, indent=2, sort_keys=True) + "\n")
if result.get("schema") != q["result_schema"]:
    raise SystemExit("unexpected runtime result schema")
if result.get("status") != "pass" or proc.returncode != 0:
    raise SystemExit(proc.returncode or 1)
if result.get("mode") != q["mode"]:
    raise SystemExit("runtime result mode does not match profile")
print(f"qualification profile PASS: {profile['id']}")
