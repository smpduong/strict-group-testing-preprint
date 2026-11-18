#!/usr/bin/env python3
"""Check repro_logs/n19/MANIFEST.txt: every run entry must have its raw log file present with a matching sha256.
Also checks the recorded source-file hashes against the working tree (reported as WARN only: sources may have moved on
since the manifest was generated; regenerate with `bash reproduce_n19.sh manifest`).
Exit 0 iff every run entry has a present, matching log. A missing log is reported as 'hash recorded, raw log missing'.
Usage: python3 tests/check_manifest.py [MANIFEST]"""
import hashlib, os, re, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
man = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "repro_logs", "n19", "MANIFEST.txt")
logdir = os.path.dirname(man)
def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()
section, bad, ok, runs = None, 0, 0, 0
for line in open(man):
    line = line.rstrip("\n")
    if line.startswith("## "): section = line; continue
    if not line or line.startswith("#"): continue
    if section and section.startswith("## source files"):
        m = re.match(r"([0-9a-f]{64})  (\S+)$", line)
        if m and os.path.exists(os.path.join(ROOT, m.group(2))) and sha(os.path.join(ROOT, m.group(2))) != m.group(1):
            print("WARN source changed since manifest: " + m.group(2))
    elif section and section.startswith("## runs"):
        parts = [x.strip() for x in line.split(" | ")]
        if len(parts) < 4: print("BAD malformed manifest line: " + line[:80]); bad += 1; continue
        name = parts[0]; m = re.match(r"([0-9a-f]{64})", parts[-1])
        if not m: print("BAD %s: no sha256 in entry" % name); bad += 1; continue
        runs += 1; log = os.path.join(logdir, name + ".log")
        if not os.path.isfile(log):
            print("MISSING %s: hash recorded, raw log missing" % name); bad += 1
        elif sha(log) != m.group(1):
            print("MISMATCH %s: sha256 of %s differs from the manifest" % (name, os.path.basename(log))); bad += 1
        else:
            ok += 1
print("manifest entries=%d  logs matching=%d  problems=%d" % (runs, ok, bad))
if runs == 0: print("BAD no run entries found"); bad += 1
sys.exit(1 if bad else 0)
