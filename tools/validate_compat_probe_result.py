#!/usr/bin/env python3
import json, sys
from pathlib import Path

SCHEMA = "fellowng.compat-probe-result.v1"
SUBSYSTEMS = {"cpu","exceptions","interrupts","chipset","dma","cia","input","storage","audio","os-runtime","application"}
STATUSES = {"PASS","FAIL","SKIP"}
BOUNDS = {"pumps","slices","frames","seconds"}

def fail(msg):
    raise SystemExit(f"compat probe result invalid: {msg}")

def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: validate_compat_probe_result.py RESULT.json")
    p=Path(sys.argv[1])
    try: d=json.loads(p.read_text())
    except Exception as e: fail(str(e))
    if d.get("schema") != SCHEMA: fail("schema")
    for k in ("probe","profile"):
        if not isinstance(d.get(k),str) or not d[k]: fail(k)
    if d.get("subsystem") not in SUBSYSTEMS: fail("subsystem")
    if d.get("status") not in STATUSES: fail("status")
    obs=d.get("observations")
    if not isinstance(obs,list): fail("observations")
    for o in obs:
        if not isinstance(o,dict) or not isinstance(o.get("name"),str) or not o["name"] or "value" not in o: fail("observation")
    if "execution_bound" in d:
        b=d["execution_bound"]
        if not isinstance(b,dict) or b.get("kind") not in BOUNDS or not isinstance(b.get("value"),int) or isinstance(b.get("value"),bool) or b["value"] < 1: fail("execution_bound")
    if d["status"] == "SKIP" and not d.get("reason"): fail("SKIP requires reason")
    print(f'{d["probe"]}: {d["status"]}')

if __name__ == "__main__": main()
