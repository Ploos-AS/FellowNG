#!/usr/bin/env python3
"""Prepare and evaluate a local FellowNG ARexx m68k qualification bundle.

Guest execution remains intentionally explicit: this tool stages the project-authored
probe, writes the RX launch command, and can parse captured guest output after the
licensed Workbench environment has executed it.
"""
from __future__ import annotations
import argparse, json, pathlib, subprocess, sys

ap = argparse.ArgumentParser()
ap.add_argument("--workbench", required=True)
ap.add_argument("--evidence-dir", required=True)
ap.add_argument("--probe", default="probes/arexx/arexx-baseline-v1.rexx")
ap.add_argument("--guest-output",
                help="captured output from the guest RX invocation; when supplied, parse and gate it")
args = ap.parse_args()

evidence = pathlib.Path(args.evidence_dir)
evidence.mkdir(parents=True, exist_ok=True)
stage_meta = evidence / "arexx-stage.json"

stage = [
    sys.executable, "tools/stage_arexx_probe.py",
    "--workbench", args.workbench,
    "--probe", args.probe,
    "--destination", "T:FellowNG-ARexx-Probe.rexx",
    "--metadata", str(stage_meta),
]
subprocess.run(stage, check=True)

meta = json.loads(stage_meta.read_text())
launch = meta["launch"]
(evidence / "guest-launch.txt").write_text(launch + "\n")
print("guest launch:", launch)

if not args.guest_output:
    print("ARexx qualification READY: execute guest-launch.txt inside the m68k Workbench guest")
    raise SystemExit(0)

raw = pathlib.Path(args.guest_output)
if not raw.is_file():
    raise SystemExit(f"guest output not found: {raw}")
captured = evidence / "arexx-guest-output.txt"
captured.write_bytes(raw.read_bytes())

result = evidence / "arexx-result.json"
parse = [
    sys.executable, "tools/parse_arexx_probe.py",
    str(captured), "--output", str(result),
]
proc = subprocess.run(parse)
if proc.returncode:
    print("ARexx qualification FAIL")
    raise SystemExit(proc.returncode)
print("ARexx qualification PASS")
