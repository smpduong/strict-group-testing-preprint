#!/usr/bin/env python3
"""Independent verifier.
Equivalence used (proved in notes): q pools P_1..P_q subset of E satisfy the requirement iff
there is a type assignment t(v) subset of {0..q-1} (t(v)={j: v in P_j}) such that for every
candidate set C and every v in E\\C:  t(v) is NOT a subset of  union_{c in C} t(c).
Here only the type-assignment search is done (plain backtracking), no coloring graph."""
import itertools, sys

def cand(sigs, W):
    E = [i for i, s in enumerate(sigs) if s & ~W == 0]
    sets = []
    if W == 0:
        sets.append(())
    for a in E:
        if sigs[a] == W:
            sets.append((a,))
    for a, b in itertools.combinations(E, 2):
        if (sigs[a] | sigs[b]) == W:
            sets.append((a, b))
    return E, sets

def r_le(E, sets, q):
    """True iff r(W) <= q for candidate structure (E, sets)."""
    reqs = [(C, v) for C in sets for v in E if v not in C]
    if not reqs:
        return True
    if not sets:
        return True
    if q == 0:
        return False
    # for each element, list of constraints (C, v) where it is v, or it is in C
    assign = {}
    order = sorted(E, key=lambda e: -sum(1 for C, v in reqs if v == e or e in C))
    dom = range(0, 1 << q)  # type 0 allowed (element in no pool); violations are caught by the constraints

    def ok_partial():
        for C, v in reqs:
            if v in assign and all(c in assign for c in C):
                U = 0
                for c in C:
                    U |= assign[c]
                if assign[v] & ~U == 0:
                    return False
        return True

    def bt(i, maxpool):
        if i == len(order):
            return True
        e = order[i]
        for t in dom:
            assign[e] = t
            if ok_partial() and bt(i + 1, maxpool):
                return True
            del assign[e]
        return False

    return bt(0, q)

def max_r_ok(sigs, k, q):
    for W in range(1 << k):
        E, sets = cand(sigs, W)
        if not r_le(E, sets, q):
            return False, W
    return True, None

if __name__ == "__main__":
    # usage: verify.py [--max-tests T] k q sig1 sig2 ...
    # ARTEFACT TYPE: checks a POSITIVE-STRATEGY WITNESS (design = signatures + q stage-2 pools per outcome) through
    # Corollary 5 (r(W) <= q for every W), independently of the search solvers.  It cannot certify an impossibility
    # (negative-search logs such as "designs_found=0" are not checkable this way).
    # --max-tests T: reject (exit 2) if the design uses k + q > T tests.
    args = sys.argv[1:]
    max_tests = None
    if args and args[0] == "--max-tests":
        if len(args) < 2 or not args[1].lstrip("-").isdigit():
            print("INVALID: --max-tests needs an integer"); sys.exit(2)
        max_tests = int(args[1]); args = args[2:]
    if len(args) < 3:
        print("usage: verify.py [--max-tests T] k q sig1 sig2 ..."); sys.exit(2)
    k = int(args[0]); q = int(args[1]); sigs = [int(x) for x in args[2:]]
    # strict input validation: reject malformed designs instead of silently mis-checking them
    def _bad(msg):
        print("INVALID:", msg); sys.exit(2)
    if k < 1 or q < 0: _bad("k must be >= 1 and q >= 0 (got k=%d q=%d)" % (k, q))
    if len(sigs) < 1: _bad("no items")
    for i, sg in enumerate(sigs):
        if sg < 0 or sg >= (1 << k): _bad("signature of item %d (%d) is not a subset of the k=%d stage-1 pools (nonexistent pool referenced)" % (i, sg, k))
    print("ARTEFACT TYPE: positive-strategy witness check (design verified via Corollary 5; not an impossibility certificate)")
    if max_tests is not None and k + q > max_tests:
        print(f"INVALID: uses k+q={k + q} tests > allowed max_tests={max_tests}", f"k={k} q={q} n={len(sigs)}")
        sys.exit(2)
    ok, W = max_r_ok(sigs, k, q)
    print("VALID" if ok else f"INVALID at W={W}", f"k={k} q={q} n={len(sigs)}", "" if max_tests is None else f"tests={k + q}<=max_tests={max_tests}")
    sys.exit(0 if ok else 1)
