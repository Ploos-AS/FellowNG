#!/usr/bin/env python3
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]

required_paths = [
    ROOT / "docs" / "M1_PORTABILITY_INVENTORY.md",
    ROOT / "fellow" / "SRC" / "WinFellow.Core",
    ROOT / "fellow" / "SRC" / "WinFellow.Hardfile",
    ROOT / "fellow" / "SRC" / "WinFellow.Core.Tests",
    ROOT / "fellow" / "SRC" / "WinFellow.Hardfile.Tests",
    ROOT / "fellow" / "SRC" / "WinFellow" / "WinFellow.vcxproj",
]

missing = [str(path.relative_to(ROOT)) for path in required_paths if not path.exists()]
if missing:
    print("M1 FAIL: required paths missing:")
    for path in missing:
        print(f"  - {path}")
    sys.exit(1)

project = (ROOT / "fellow" / "SRC" / "WinFellow" / "WinFellow.vcxproj").read_text(
    encoding="utf-8-sig", errors="replace"
).lower()

expected_windows_dependencies = [
    "ddraw.lib",
    "dinput8.lib",
    "dsound.lib",
    "dxgi.lib",
    "d3d11.lib",
    "winmm.lib",
]

missing_dependencies = [dep for dep in expected_windows_dependencies if dep not in project]
if missing_dependencies:
    print("M1 FAIL: inventory is stale; expected Windows dependencies not found:")
    for dep in missing_dependencies:
        print(f"  - {dep}")
    sys.exit(1)

report = (ROOT / "docs" / "M1_PORTABILITY_INVENTORY.md").read_text(encoding="utf-8")
required_report_terms = [
    "M2 — Portable core build",
    "Linux x86-64",
    "CMake",
    "WinFellow.Core",
    "WinFellow.Hardfile",
    "Direct3D 11",
    "DirectInput",
    "DirectSound",
]
missing_terms = [term for term in required_report_terms if term not in report]
if missing_terms:
    print("M1 FAIL: portability report is incomplete:")
    for term in missing_terms:
        print(f"  - {term}")
    sys.exit(1)

print("M1 PASS: portability inventory baseline is present and internally consistent")
