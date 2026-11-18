#!/usr/bin/env python3
"""Standalone brute-force checker for a 2-stage strict group-testing certificate.

ARTEFACT TYPE: this checks a POSITIVE-STRATEGY WITNESS (a concrete design + stage-2 pools), independently of the
search code. It cannot check a negative-search log ("designs_found=0"): impossibility claims are NOT certified here.

Usage: python3 verify.py cert.json [max_t]
  max_t (optional, integer): the strategy must use at most max_t tests in the worst case (k stage-1 pools + the largest
  stage-2 pool list); otherwise the certificate is rejected (exit code 2, "INVALID: uses ... > max_t").
Certificate: {"n","k","sigs":[[pools of item i]...],"stage2":{"<W mask>":[[items of pool]...]}}
W mask = bitmask over stage-1 pools of the positive ones.
Checks EVERY subset P of items (all 2^n) for n<=22, else every |P|<=4.
Why sets of size <=3 are enough (strict model): the strategy is valid iff every P with |P|<=2 has a transcript
(stage-1 outcome W, then the outcomes of the stage-2 pools chosen for W) that no other set Q != P has. Suppose Q != P
collides with P, |P|<=2. If Q is a proper subset of P it is itself small (size <=1). Otherwise pick x in Q\\P and set
P' = P+{x}, |P'|<=3, P' != P. Every pool (stage-1 or stage-2) containing x is positive for Q, hence for P (same
transcript), so P' gives exactly P's outcome in stage 1 (same W, same stage-2 pools) and in stage 2: P' collides with P.
So a violation exists iff one exists among sets of size <=3; the n>22 mode (size <=4) is complete, and sets of size >=5
need not be enumerated. (Reachable W only: a W reached solely by larger sets cannot occur in any collision with a small P.)
(1) decoding rule: discard items in any negative pool (stage1+stage2); R=remaining;
    output R as defective set iff R is a candidate set (|R|<=2 and union of signatures == W), else '|P|>2'.
    Required: P==R output when |P|<=2, and '|P|>2' when |P|>=3.
(2) independent check: outcome vectors (full 2-stage transcripts) of all |P|<=2 are pairwise distinct
    and differ from those of every |P|>=3 (information-theoretic correctness).
Exit codes: 0 VALID; 1 INVALID strategy; 2 INVALID because of the max_t budget or a malformed certificate."""
import sys, json, itertools
def die(msg, code=2):
    print("INVALID:", msg)
    sys.exit(code)
if len(sys.argv) < 2 or len(sys.argv) > 3:
    die("usage: verify.py cert.json [max_t]")
max_t = None
if len(sys.argv) == 3:
    try:
        max_t = int(sys.argv[2])
    except ValueError:
        die("max_t must be an integer, got %r" % sys.argv[2])
def _no_dup_keys(pairs):
    seen = set()
    for key, _ in pairs:
        if key in seen: die("malformed certificate: duplicate JSON key %r" % key)
        seen.add(key)
    return dict(pairs)
def _isint(x): return isinstance(x, int) and not isinstance(x, bool)
try:
    cert = json.load(open(sys.argv[1]), object_pairs_hook=_no_dup_keys)
except (OSError, ValueError) as e:
    die("cannot read certificate: %s" % e)
print("ARTEFACT TYPE: positive-strategy witness (checked independently of the search code; a negative-search log cannot be checked this way)")
if not isinstance(cert, dict) or not all(x in cert for x in ("n", "k", "sigs", "stage2")): die("malformed certificate: need n, k, sigs, stage2")
print("certificate label:", cert.get("artefact", "(none; unlabelled certificate)"))
n, k = cert["n"], cert["k"]
if not _isint(n) or not _isint(k) or n < 1 or k < 0: die("n and k must be integers (n>=1, k>=0)")
if not isinstance(cert["sigs"], list) or not isinstance(cert["stage2"], dict): die("malformed certificate: sigs must be a list, stage2 an object")
# strict structural validation (duplicates would otherwise be silently summed into carries by the bitmask encoding)
if len(cert["sigs"]) != n: die("declared n=%d but sigs lists %d items" % (n, len(cert["sigs"])))
for i, s_ in enumerate(cert["sigs"]):
    if not isinstance(s_, list): die("signature of item %d is not a list" % i)
    if any((not _isint(j)) or j < 0 or j >= k for j in s_): die("signature of item %d uses a pool index outside 0..k-1" % i)
    if len(set(s_)) != len(s_): die("signature of item %d lists a stage-1 pool twice" % i)
st2 = {}
for w, pools in cert["stage2"].items():
    try: wi = int(w)
    except (TypeError, ValueError): die("stage2 key %r is not an integer W mask" % (w,))
    if str(wi) != w: die("stage2 key %r is not in canonical decimal form" % (w,))
    if not (0 <= wi < (1 << k)): die("stage2 key %d is not a W mask over k=%d pools" % (wi, k))
    if wi in st2: die("stage2 has two entries for outcome W=%d" % wi)
    if not isinstance(pools, list): die("stage2[%s] is not a list of pools" % w)
    masks = []
    for p in pools:
        if not isinstance(p, list): die("stage2[%s] contains a pool that is not a list" % w)
        if any(not _isint(x) or x < 0 or x >= n for x in p): die("stage-2 pool for W=%d references a nonexistent item (outside 0..n-1)" % wi)
        if len(set(p)) != len(p): die("stage-2 pool for W=%d lists an item twice" % wi)
        masks.append(sum(1 << x for x in p))
    if len(set(masks)) != len(masks): die("stage2 for W=%d contains the same pool twice" % wi)
    st2[wi] = masks
sig = [sum(1 << j for j in s) for s in cert["sigs"]]
stage1 = [sum(1 << i for i in range(n) if sig[i] >> j & 1) for j in range(k)]
total = k + max([len(v) for v in st2.values()] + [0])
print("n=%d k=%d stage1 pools sizes=%s max stage2 pools=%d total tests=%d" % (
    n, k, [bin(p).count("1") for p in stage1], total - k, total))
if max_t is not None and total > max_t:
    die("uses %d tests (k=%d + %d stage-2) > allowed max_t=%d" % (total, k, total - k, max_t))
def cands(W):
    E = [i for i in range(n) if sig[i] & ~W == 0]
    out = []
    for r in range(3):
        for C in itertools.combinations(E, r):
            u = 0
            for c in C: u |= sig[c]
            if u == W: out.append(sum(1 << c for c in C))
    return set(out)
cand_cache = {}
full = (1 << n) - 1
limit = n if n <= 22 else 4
seen = {}
bad = 0
cnt = 0
for size in range(0, limit + 1):
    for P in itertools.combinations(range(n), size):
        Pm = sum(1 << i for i in P)
        W = sum(1 << j for j in range(k) if stage1[j] & Pm)
        if W not in st2:
            bad += 1; print("FAIL no stage-2 pools given for reachable outcome W=%d (P=%s)" % (W, P)); continue
        pools = st2[W]
        out2 = tuple(1 if p & Pm else 0 for p in pools)
        R = full
        for j in range(k):
            if not (W >> j & 1): R &= ~stage1[j]
        for p, o in zip(pools, out2):
            if not o: R &= ~p
        if W not in cand_cache: cand_cache[W] = cands(W)
        output = R if R in cand_cache[W] else None
        if size <= 2:
            if output != Pm: bad += 1; print("FAIL decode P=", P, "R=", bin(R))
        else:
            if output is not None: bad += 1; print("FAIL |P|>2 reported as set P=", P)
        vec = (W, out2)
        seen.setdefault(vec, []).append(size)
        cnt += 1
# transcript distinctness
for vec, sizes in seen.items():
    small = sum(1 for s in sizes if s <= 2)
    if small > 1 or (small == 1 and len(sizes) > 1):
        bad += 1; print("FAIL transcript collision", vec, sizes)
print("checked %d defective sets (|P|<=%d), failures=%d" % (cnt, limit, bad))
print("VALID: strict 2-stage strategy with %d tests%s" % (total, "" if max_t is None else " (<= max_t=%d)" % max_t) if bad == 0 else "INVALID")
sys.exit(1 if bad else 0)
