#!/usr/bin/env python3
"""Independent brute-force reference (no symmetry breaking, no coloring graph).
r(W) computed directly from the pool definition by enumerating pool families."""
import os, itertools, random, subprocess, sys

GT2 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "gt2")   # build: gcc -O2 -o independent/gt2 independent/gt2.c

def r_def(E, sets, cap=6):
    """min number of pools (subsets of E) meeting the definition; cap if > cap."""
    reqs = [(C, v) for C in sets for v in E if v not in C]
    if not reqs:
        return 0
    subs = []
    for r in range(1, len(E) + 1):
        for comb in itertools.combinations(E, r):
            subs.append(frozenset(comb))
    for q in range(1, cap + 1):
        for pools in itertools.combinations(subs, q):
            if all(any(v in P and not (P & C) for P in pools) for (C, v) in reqs):
                return q
    return cap + 1

def cand(sigs, W):
    E = [i for i, s in enumerate(sigs) if s & ~W == 0]
    sets = []
    if W == 0:
        sets.append(frozenset())
    for a in E:
        if sigs[a] == W:
            sets.append(frozenset([a]))
    for a, b in itertools.combinations(E, 2):
        if (sigs[a] | sigs[b]) == W:
            sets.append(frozenset([a, b]))
    return E, sets

def max_r(sigs, k):
    m = 0
    for W in range(1 << k):
        E, sets = cand(sigs, W)
        if not sets:
            continue
        m = max(m, r_def(E, sets))
    return m

def chi_c(m, sets):
    inp = f"{m} {len(sets)} " + " ".join(str(sum(1 << e for e in C)) for C in sets) + "\n"
    out = subprocess.run([GT2, "chi"], input=inp, capture_output=True, text=True).stdout
    return int(out.strip())

def test_chi(trials=300, seed=1):
    rnd = random.Random(seed)
    bad = 0
    for t in range(trials):
        m = rnd.randint(1, 5)
        allsets = [frozenset([i]) for i in range(m)] + [frozenset(c) for c in itertools.combinations(range(m), 2)]
        allsets.append(frozenset())
        N = rnd.randint(1, min(len(allsets), 6))
        sets = rnd.sample(allsets, N)
        # reference: the chromatic number equals min pools meeting the requirement
        ref = r_def(list(range(m)), sets, cap=8)
        got = chi_c(m, sets)
        if ref != got:
            bad += 1
            print("MISMATCH", m, [sorted(C) for C in sets], ref, got)
    print(f"chi cross-check: {trials} random families, mismatches={bad}")
    return bad

def exists_brute(n, T, kmax=None):
    """returns dict k -> min over multisets of max_r (capped), for k in 1..T"""
    best = {}
    for k in range(1, T + 1):
        b = 10**9
        for ms in itertools.combinations_with_replacement(range(1 << k), n):
            sigs = list(ms)
            mr = max_r(sigs, k)
            if mr < b:
                b = mr
            if k + b <= T and False:
                break
        best[k] = b
    return best

def feasible_min_T(best):
    return min(k + best[k] for k in best)

if __name__ == "__main__":
    mode = sys.argv[1] if len(sys.argv) > 1 else "chi"
    if mode == "chi":
        test_chi()
    elif mode == "small":
        n = int(sys.argv[2]); T = int(sys.argv[3])
        best = exists_brute(n, T)
        print(f"n={n} best max_r per k: {best}  min total T = {feasible_min_T(best)}")
