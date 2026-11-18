> **Current preprint clarification (18 November 2026):** Historical text below retains its original nonadaptive-capacity argument. In the current manuscript, the n19 and n28/n29 nonadaptive endpoints follow directly from the pair-antichain bound; g(9)<=12 is not a premise. The n28/n29 theorem remains conditional on f(9)<=18. See manuscript.pdf and STATUS.md.

> Historical conditional argument. See ../STATUS.md for current qualification and manuscript for the reviewed statement.

# n=29 with 10 tests (2026-10-18)

> **Historical note (added 2026-10-19).** This report is kept as written. Its statement that f(10) "is now known only to lie in [24, 28]" is superseded: n=28 (`n28/REPORT.md`) and, conditionally, n=27 (`n27/NOTES.md`) have since been excluded, so f(10) is in [24, 26] (conditional on t(19,2,2)=10; n=27 also rests on a negative search that two implementations agree has no design, with the second-solver cross-check since completed; the logs are not independently checkable certificates). Notation: the deficiency d(W) here is delta(W) in `docs/note-draft.md`; Lemmas A and B are as defined there (Section 1.3). t(29,2,2)=11 is a conditional candidate. The "re-run" and "identical node counts" in the checks below compare re-runs of the same code and are consistency checks only.

**Claim (candidate, not peer reviewed):** no strict two-stage strategy with 10 tests exists for n=29, so t(29,2,2) >= 11. With the DSW 2014 upper bound t(31,2,2) <= 11 this gives t(29,2,2) = 11.

**Dependency:** the proof uses f(9) <= 18, that is t(19,2,2) >= 10. That is this repository's own candidate result (`docs/results19.md`, `docs/AUDIT-n19.md`). With only published values (t(20,2,2)=10, Grankvist 2017), the same counting does not give a contradiction.

**Status of the parts:** the counting argument below is by hand, checked by the search agent and re-checked by the orchestrator (arithmetic in `count_bound.py`). The k=10 case needs g(9)=12, re-run here with `cf 13 9` (647,436,550 nodes, no family of 13).

## Setting

There are T=10 tests. Stage 1 has k pools. Stage 2 has q = 10-k pools, chosen after seeing every stage-1 outcome. S_x is item x's stage-1 signature. For a 2-set {x,y}, W = S_x ∪ S_y. p(W) is the number of 2-sets with union W.

## Proof

1. **Lemma A (one negative pool).** If pool j tests negative, the items outside it, together with the other 9 tests, form a 9-test strategy. So 29 - a_j <= f(9) = 18, which means a_j >= 11. Pool j therefore meets at least H = C(29,2) - C(18,2) = 253 of the 2-sets.
2. **Lemma B (Sperner).** For a fixed W, the 2-sets with union W have pairwise incomparable stage-2 outcome vectors. So p(W) <= B = C(q, floor(q/2)).
3. **k = 1..7.** Pool j meets at most 2^(k-1)·B 2-sets. That bound is 126, 140, 140, 160, 160, 192, 192, all below 253. Impossible.
4. **k = 8 (B=2) and k = 9 (B=1).** Here 2^(k-1)·B = 256. Define the deficiency d(W) = B - p(W) >= 0.
   - For each pool j, the deficiencies of the outcomes containing j sum to at most 256 - 253 = 3.
   - Summing over the k pools gives Σ_W |W|·d(W) <= 3k, which is 24 for k=8 and 27 for k=9.
   - Every nonempty W has |W| >= 1, so Σ_W |W|·d(W) >= (2^k - 1)·B - C(29,2). That is 104 for k=8 and 105 for k=9.
   - The upper and lower bounds contradict each other.
5. **k = 10 (one stage; k = 0 is the same).** A strict nonadaptive design is a 2-cover-free family. Pick a point p. The sets that avoid p, and the sets that contain p with p removed, are each 2-cover-free on 9 points. So the family has at most 2·g(9) = 24 < 29 members.

Larger pools (a_j > 11) only increase H, so they make step 4 worse.

## Not settled

The same counting does not exclude n = 25..28. f(10), the largest n that 10 tests can handle, is now known only to lie in [24, 28].

## Checks run by the search agent

- **Lemma B on found designs** (`lemmaB_check.py`): 18,139 exhaustively found designs (n=9..16, T=7..10, including k=8 and k=9). 0 violations, and the bound is attained.
- **f(8) = 13 and f(9) <= 18 re-runs:** node counts identical to `docs/note-draft.md` and `docs/results19.md`.
- **Positive controls:** designs found at n=16 with k=8 (T=10, brute-force VALID), n=14 with k=8, and n=12 with k=9.
- **Direct n=29 search:** killed unfinished after about 16 min. Nothing is claimed from it.
- **Code:** `gt.c` and `exh7.c` here are copies of the repo code. Only the array sizes changed, plus the FT9 and FTMAX switches.
