#!/usr/bin/env python3
"""Self-tests for FellowNG evidence manifest/validation tooling."""
from __future__ import annotations
import json, pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]
PY = sys.executable
BUILD = ROOT / "tools/build_evidence_manifest.py"
VALIDATE = ROOT / "tools/validate_evidence_bundle.py"

def seed(root: pathlib.Path, status: str = "pass", exit_status: int = 0) -> None:
    (root / "runtime-result.json").write_text(json.dumps({
        "schema": "fellowng.runtime-result.v1", "status": status}) + "\n")
    (root / "qualification-metadata.json").write_text("{}\n")
    (root / "exit-status.txt").write_text(f"{exit_status}\n")
    (root / "fellowng.log").write_text("test evidence\n")
    subprocess.run([PY, str(BUILD), str(root)], check=True,
                   stdout=subprocess.DEVNULL)

def valid(root: pathlib.Path) -> bool:
    return subprocess.run([PY, str(VALIDATE), str(root)],
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL).returncode == 0

with tempfile.TemporaryDirectory() as td:
    root = pathlib.Path(td)
    seed(root)
    assert valid(root), "positive evidence bundle must pass"

with tempfile.TemporaryDirectory() as td:
    root = pathlib.Path(td)
    seed(root)
    (root / "fellowng.log").write_text("tampered\n")
    assert not valid(root), "tampered evidence must fail"

with tempfile.TemporaryDirectory() as td:
    root = pathlib.Path(td)
    seed(root)
    (root / "fellowng.log").unlink()
    assert not valid(root), "missing required evidence must fail"

with tempfile.TemporaryDirectory() as td:
    root = pathlib.Path(td)
    seed(root, status="fail", exit_status=1)
    assert not valid(root), "runtime failure must fail validation"

print("evidence tooling tests PASS")
