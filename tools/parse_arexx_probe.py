#!/usr/bin/env python3
"""Parse FellowNG ARexx m68k baseline probe output into machine-readable evidence."""
from __future__ import annotations
import argparse, json, pathlib, re, sys

BEGIN = "FELLOWNG_AREXX_PROBE_V1 BEGIN"
PASS = "FELLOWNG_AREXX_PROBE_V1 PASS"
FAIL_PREFIX = "FELLOWNG_AREXX_PROBE_V1 FAIL"
TEST_RE = re.compile(r"^TEST\s+(\S+)\s+(PASS|FAIL)(?:\s+(.*))?$")

ap = argparse.ArgumentParser()
ap.add_argument("input", help="captured guest ARexx output")
ap.add_argument("--output", default="-", help="JSON evidence path, or - for stdout")
args = ap.parse_args()

path = pathlib.Path(args.input)
if not path.is_file():
    raise SystemExit(f"probe output not found: {path}")
lines = [line.strip() for line in path.read_text(errors="replace").splitlines()]

tests = []
seen_begin = False
seen_pass = False
guest_fail = None
for line in lines:
    if line == BEGIN:
        seen_begin = True
        continue
    if line == PASS:
        seen_pass = True
        continue
    if line.startswith(FAIL_PREFIX):
        guest_fail = line
        continue
    m = TEST_RE.match(line)
    if m:
        tests.append({"id": m.group(1), "status": m.group(2).lower(),
                      "reason": m.group(3) or None})

failed = [t for t in tests if t["status"] == "fail"]
status = "pass" if seen_begin and seen_pass and not guest_fail and tests and not failed else "fail"
reasons = []
if not seen_begin:
    reasons.append("missing begin marker")
if not tests:
    reasons.append("no test records")
if failed:
    reasons.append("one or more tests failed")
if guest_fail:
    reasons.append("guest emitted final fail marker")
if not seen_pass:
    reasons.append("missing final pass marker")

result = {
    "schema": "fellowng.arexx-result.v1",
    "probe": "FELLOWNG_AREXX_PROBE_V1",
    "status": status,
    "tests": tests,
    "summary": {"total": len(tests), "passed": len(tests) - len(failed),
                "failed": len(failed)},
    "reasons": reasons,
}

encoded = json.dumps(result, indent=2, sort_keys=True) + "\n"
if args.output == "-":
    sys.stdout.write(encoded)
else:
    pathlib.Path(args.output).write_text(encoded)
raise SystemExit(0 if status == "pass" else 1)
