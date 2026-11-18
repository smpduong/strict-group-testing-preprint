#!/usr/bin/env python3
"""Tests for search/verify.py and independent/verify.py.
 1. max_t is enforced (exit 2 when the strategy uses more tests than allowed; exit 0 when it fits).
 2. known-good certificates are VALID, the corrupted one is INVALID, a certificate with a missing stage-2 entry is INVALID.
 3. Empirical support for "sets of size <= 3 suffice": on random mutations of a valid certificate, the verdict
    'a set of size <=2 shares its transcript with another set' computed from sets of size <=3 only equals the verdict
    computed from ALL 2^n sets (the proof is in docs/VERIFIER-justification.md).
Run from the repo root:  python3 tests/test_verify.py
"""
import json, os, random, subprocess, sys, itertools, tempfile
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
V1 = os.path.join(ROOT, "search", "verify.py")
V2 = os.path.join(ROOT, "independent", "verify.py")
fails = 0
def check(name, cond):
    global fails
    print(("PASS " if cond else "FAIL ") + name)
    if not cond: fails += 1
def run(args):
    p = subprocess.run([sys.executable, "-I"] + args, capture_output=True, text=True)
    return p.returncode, p.stdout
K13 = os.path.join(ROOT, "search", "cert_n13_known.json")
K18 = os.path.join(ROOT, "search", "cert_n18_known.json")
BAD = os.path.join(ROOT, "search", "cert_n13_corrupt.json")

# 1. max_t enforcement (search/verify.py)
rc, out = run([V1, K13]);        check("search/verify.py: no max_t, valid cert -> exit 0", rc == 0 and "VALID" in out)
rc, out = run([V1, K13, "8"]);   check("search/verify.py: max_t=8 (cert uses 8) -> exit 0", rc == 0)
rc, out = run([V1, K13, "7"]);   check("search/verify.py: max_t=7 (cert uses 8) -> exit 2 INVALID", rc == 2 and "INVALID" in out and "max_t=7" in out)
rc, out = run([V1, K18, "8"]);   check("search/verify.py: n=18 cert uses 9, max_t=8 -> exit 2", rc == 2)
rc, out = run([V1, K18, "9"]);   check("search/verify.py: n=18 cert, max_t=9 -> exit 0", rc == 0)
rc, out = run([V1, K13, "x"]);   check("search/verify.py: non-integer max_t -> exit 2", rc == 2)
rc, out = run([V1, BAD]);        check("search/verify.py: corrupted cert -> exit 1 INVALID", rc == 1 and "INVALID" in out)
check("search/verify.py: output labels the artefact as a positive-strategy witness", "positive-strategy witness" in run([V1, K13])[1])
# missing stage-2 entry for a reachable outcome
c = json.load(open(K13)); c["stage2"].pop("3")
with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False) as f: json.dump(c, f); tmp = f.name
rc, out = run([V1, tmp]); os.unlink(tmp)
check("search/verify.py: missing stage-2 entry for reachable W -> INVALID (no crash)", rc == 1 and "INVALID" in out and "Traceback" not in out)

# max-tests for independent/verify.py: Prop-15 n=13 design is k=5,q=3 (8 tests)
sig13 = [(1 << a) | (1 << b) for a, b in itertools.combinations(range(5), 2)] + [0, 0, 0]
args = [str(x) for x in sig13]
rc, out = run([V2, "5", "3"] + args);                       check("independent/verify.py: valid design, exit 0", rc == 0 and out.count("VALID") >= 1 and "INVALID" not in out)
rc, out = run([V2, "--max-tests", "8", "5", "3"] + args);   check("independent/verify.py: --max-tests 8 ok", rc == 0)
rc, out = run([V2, "--max-tests", "7", "5", "3"] + args);   check("independent/verify.py: --max-tests 7 -> exit 2", rc == 2 and "INVALID" in out)
rc, out = run([V2, "5", "2"] + args);                       check("independent/verify.py: q=2 too small -> INVALID exit 1", rc == 1 and "INVALID" in out)
check("independent/verify.py: output labels the artefact", "positive-strategy witness" in run([V2, "5", "3"] + args)[1])

# 3. sizes <= 3 suffice (empirical), on mutated certificates
def verdict(cert, maxsize):
    n, k = cert["n"], cert["k"]
    sig = [sum(1 << j for j in s) for s in cert["sigs"]]
    st2 = {int(w): [sum(1 << i for i in p) for p in pools] for w, pools in cert["stage2"].items()}
    stage1 = [sum(1 << i for i in range(n) if sig[i] >> j & 1) for j in range(k)]
    seen = {}
    for size in range(maxsize + 1):
        for P in itertools.combinations(range(n), size):
            m = sum(1 << i for i in P)
            W = sum(1 << j for j in range(k) if stage1[j] & m)
            pools = st2.get(W)
            if pools is None: return "missing"
            key = (W, tuple(1 if p & m else 0 for p in pools))
            seen.setdefault(key, []).append(size)
    for sizes in seen.values():
        small = sum(1 for s in sizes if s <= 2)
        if small > 1 or (small == 1 and len(sizes) > 1): return "collision"
    return "ok"
rnd = random.Random(5); base = json.load(open(K13)); agree = 0; kinds = {"ok": 0, "collision": 0, "missing": 0}
for t in range(150):
    c = json.loads(json.dumps(base))
    for _ in range(rnd.randint(1, 3)):
        W = rnd.choice([w for w, p in c["stage2"].items() if p])
        pools = c["stage2"][W]; r = rnd.random()
        if r < 0.4 and len(pools) > 1: pools.pop(rnd.randrange(len(pools)))
        elif r < 0.8 and pools:
            p = rnd.choice(pools)
            if len(p) > 1: p.pop(rnd.randrange(len(p)))
        else:
            p = rnd.choice(pools); x = rnd.randrange(c["n"])
            if x not in p: p.append(x)
    a, b = verdict(c, 3), verdict(c, c["n"])
    kinds[b] += 1
    if a == b: agree += 1
check("sizes<=3 verdict == all-2^n verdict on 150 mutated certificates (%s)" % kinds, agree == 150 and kinds["collision"] > 10)
# 4. crafted bad certificates (structural validation): every one must FAIL (nonzero exit, INVALID, no traceback)
import copy
def mutated(fn, base_path=K13):
    c = json.load(open(base_path)); fn(c)
    with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False) as f: json.dump(c, f); tmp = f.name
    r = run([V1, tmp]); os.unlink(tmp); return r
def first_nonempty_w(c): return next(w for w, p in c["stage2"].items() if p and len(p[0]) > 1)
def dup_sig(c): c["sigs"][0] = c["sigs"][0] + [c["sigs"][0][0]]
def dup_item_in_pool(c): w = first_nonempty_w(c); c["stage2"][w][0].append(c["stage2"][w][0][0])
def oor_item(c): w = first_nonempty_w(c); c["stage2"][w][0].append(c["n"] + 5)
def neg_item(c): w = first_nonempty_w(c); c["stage2"][w][0].append(-1)
def bad_pool_idx(c): c["sigs"][0] = c["sigs"][0] + [c["k"]]
def n_too_big(c): c["n"] += 1
def n_too_small(c): c["n"] -= 1
def dup_pool(c): w = first_nonempty_w(c); c["stage2"][w].append(list(c["stage2"][w][0]))
def alias_key(c): w = first_nonempty_w(c); c["stage2"]["0" + w] = c["stage2"][w]
def bad_key(c): c["stage2"]["abc"] = []
def key_range(c): c["stage2"][str(1 << c["k"])] = []
def str_item(c): w = first_nonempty_w(c); c["stage2"][w][0][0] = "1"
def bool_item(c): w = first_nonempty_w(c); c["stage2"][w][0][0] = True
def pool_not_list(c): w = first_nonempty_w(c); c["stage2"][w][0] = 3
for name, fn in [("pool lists a stage-1 pool twice in an item signature", dup_sig), ("stage-2 pool lists an item twice", dup_item_in_pool),
                 ("stage-2 item out of range (>= n)", oor_item), ("negative item index", neg_item), ("signature references nonexistent pool", bad_pool_idx),
                 ("declared n larger than items used", n_too_big), ("declared n smaller than items used", n_too_small),
                 ("same stage-2 pool listed twice", dup_pool), ("two stage-2 entries for the same outcome (alias key)", alias_key),
                 ("non-numeric stage-2 key", bad_key), ("stage-2 key out of W range", key_range),
                 ("string item index", str_item), ("boolean item index", bool_item), ("pool not a list", pool_not_list)]:
    rc, out = mutated(fn)
    check("search/verify.py rejects: " + name, rc != 0 and "INVALID" in out and "Traceback" not in out and "VALID: strict" not in out)
# duplicate JSON keys (conflicting stage-2 entries for one outcome, later one silently wins in plain json.load)
raw = open(K13).read(); w0 = first_nonempty_w(json.load(open(K13)))
dupraw = raw.replace('"stage2":{', '"stage2":{"%s":[],' % w0, 1) if '"stage2":{' in raw else raw.replace('"stage2": {', '"stage2": {"%s": [],' % w0, 1)
with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False) as f: f.write(dupraw); tmp = f.name
rc, out = run([V1, tmp]); os.unlink(tmp)
check("search/verify.py rejects: duplicate JSON key for one stage-2 outcome", dupraw != raw and rc == 2 and "duplicate" in out and "Traceback" not in out)
for name, content in [("not JSON", "{oops"), ("not an object", "[1,2]"), ("missing fields", "{\"n\":3}")]:
    with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False) as f: f.write(content); tmp = f.name
    rc, out = run([V1, tmp]); os.unlink(tmp)
    check("search/verify.py rejects: " + name, rc == 2 and "INVALID" in out and "Traceback" not in out)
# all stored certificates (except the deliberately corrupted one) still PASS
import glob
stored = sorted(glob.glob(os.path.join(ROOT, "**", "*.json"), recursive=True))
n_ok = 0
for pth in stored:
    try: d = json.load(open(pth))
    except Exception: continue
    if not (isinstance(d, dict) and "stage2" in d) or pth == BAD: continue
    rc, out = run([V1, pth]); n_ok += 1
    check("stored certificate %s -> VALID" % os.path.relpath(pth, ROOT), rc == 0 and "VALID: strict" in out)
check("found >= 3 stored certificates", n_ok >= 3)
# independent/verify.py: malformed designs must fail
for name, a in [("signature outside 0..2^k-1", ["5", "3"] + [str(x) for x in sig13[:-1]] + ["99"]), ("negative signature", ["5", "3"] + [str(x) for x in sig13[:-1]] + ["-1"]),
                ("k=0", ["0", "3", "0", "0"]), ("negative q", ["5", "-1"] + args)]:
    rc, out = run([V2] + a)
    check("independent/verify.py rejects: " + name, rc != 0 and "INVALID" in out and "Traceback" not in out)
print("FAILURES:", fails); sys.exit(1 if fails else 0)
