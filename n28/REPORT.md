> **Current preprint clarification (18 November 2026):** Historical text below retains its original nonadaptive-capacity argument. In the current manuscript, the n19 and n28/n29 nonadaptive endpoints follow directly from the pair-antichain bound; g(9)<=12 is not a premise. The n28/n29 theorem remains conditional on f(9)<=18. See manuscript.pdf and STATUS.md.

# n=28 with 10 tests (2026-10-19)

> **Historical note (added 2026-10-19, later).** This report is kept as written. The range "f(10) is in [24, 27]" below and the statement that n=27 is "still open" were true when written and are superseded: n=27 has since been reduced to a negative search, giving the conditional candidate t(27,2,2)=11 and f(10) in [24, 26] (`n27/NOTES.md`, `docs/note-draft.md` Section 5; the gt2p second-solver cross-check of n=27 has since completed with 0 designs, in agreement with exh7). Notation: the deficiency d(W) here is called delta(W) in `docs/note-draft.md`, and "Lemma A/B" are as defined there (Section 1.3). t(28,2,2)=11 is a conditional candidate (on t(19,2,2)=10 and g(9)=12).

**Claim (candidate, not peer reviewed):** no strict two-stage strategy with 10 tests exists for n=28, so t(28,2,2) >= 11. With DSW 2014 Prop. 15 (t(31,2,2) <= 11), t(28,2,2) = 11, and f(10) is in [24, 27].

**Dependency:** like n=29, this uses f(9) <= 18, i.e. our candidate t(19,2,2) = 10. For k=10 it uses g(9) = 12 (the largest 2-cover-free family on 9 points).

**Origin and checks:** a Sonnet planning agent found the argument (`wdef.py`). A Haiku agent, which did not read that script, recomputed every number (`derive_independent.py`), and the two agree. Opus checked the argument by hand. No search was run.

## Argument

The notation is the same as `n29/REPORT.md`: H(a) = C(28,2) - C(28-a,2), B = C(q, floor(q/2)), and d(W) = B - p(W) is in [0, B].

1. **Lemma A.** Every pool has a_j >= 28 - 18 = 10, so it meets H >= 225 of the 2-sets.
2. **k = 1..7.** The Lemma B cap 2^(k-1)·B is at most 192, which is below 225. Excluded.
3. **k = 10 (and k = 0).** The family has at most 2·g(9) = 24 < 28 members. Excluded.
4. **k = 8, 9 (cap 256).** For each pool j, the deficiencies of the outcomes containing j sum to 256 - H(a_j), which is at most 31. Summing over the pools gives Σ_W |W|·d(W) <= 31k.
5. **Lower bound for step 4.** We need Σ_{W≠∅} d(W) >= (2^k - 1)·B - C(28,2). The weighted sum is smallest when the deficiency sits on the smallest W first:
   - k=8: the sum must reach 132. The smallest weighted sum is 8·2·1 + 28·2·2 + 30·2·3 = 308, above the upper bound of 248.
   - k=9: the sum must reach 133. The smallest weighted sum is 9 + 72 + 252 + 4·4 = 349, above the upper bound of 279.
   Both are contradictions.

The n=29 proof is the same argument with the weaker bound |W| >= 1.

## Controls

- n=18, T=9, k=6 (a design exists): the bounds are equal (54 vs 54), so the design is not wrongly excluded.
- n=24, T=10 (a design exists, k=7): not excluded.
- n=19, T=9, k=5 and 6: not excluded by counting, which agrees with the search-based proof.

## Not settled

n=27 is still open for k=8 and 9 (k=8: lower bound 389 vs upper bound 464; k=9: 457 vs 522). n=25 and 26 are open for more k. A possible next step is to add the per-outcome caps m_W <= f(q+|W|) as a linear program before any search.

Release correction (2026-11-18): the last term represents 30 triple outcomes, each with deficit capacity 2 and weight 3. Original term 30·3 omitted the factor 2. The corrected total is 308. Conditional assumptions remain required.
