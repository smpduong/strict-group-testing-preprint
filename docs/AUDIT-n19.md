> Historical source document, retained for context. Current release status is in ../README.md. AI review only; no independent human review. Links may refer to the full private research repository, not this curated subset.

*Independent audit, performed 2026-10-18. Verbatim copy of the auditor's report.*

# Audit: "no 2-stage strict d=2 strategy with 9 tests for n=19, hence t(19,2,2)=10"

Auditor's own code (all written from scratch, none of the repo's solvers reused for the spot checks) is in this folder:
core.hpp (own r(W) solvers, semantic brute force), spot_a*.cpp / cmp_a.py / mine_feas.cpp / wrap1.c / wrap2.c (solver cross-checks),
verify_full.cpp (full 2^n simulation), add19*.cpp (19th item), mysearch.cpp (own exhaustive search), mycff.cpp (own 2-cover-free search).
Logs: my19_k*.txt, my14_*.txt, cff_9_*.txt, d_exh7.txt.

## Checklist

| # | Item | Verdict |
|---|------|---------|
| 1 | Model: cost = k + max_W r(W) is an exact characterisation | PASS |
| 2 | Coverage of the case split k=1..9 (k=0 not needed) | PASS |
| 3a | Lemma A (negative-set restriction) | PASS (conditional on f(8)=13, see 3d) |
| 3b | Lemma B (Sperner antichain), incl. pairs with one item in pool j | PASS |
| 3c | k=1..4 arithmetic and pool windows (k=6: exactly 6; k=7,8: 6..9; INTERCAP=2) | PASS |
| 3d | Dependence f(8)=13 <- t(14,2,2)=9; f(1..7) | PASS with a GAP on f(7)=9 (taken from the paper, not re-derived) |
| 4 | Prunes and symmetries, completeness | PASS (no unestablished assumption found); two minor GAPs/doc defects listed |
| 5 | k=9 / cf.c / 12 items | PASS |
| 6a | r(W) brute force vs both implementations | PASS |
| 6b | Re-derivation of f(0..7) | PASS for f(0..6); GAP for f(7); f(8) independently supported (n=14 runs) |
| 6c | Full simulation of valid n=18 designs | PASS |
| 6d | 19th item of every signature | PASS (no signature works) |
| 7 | Independent re-run of the n=19 search | PASS for k=6,7,8 (identical node counts); GAP for k=5 (not re-run by me) |
| D | Documentation consistency | FAIL (minor, does not affect the conclusion) |

## 1. Model (PASS)

Argument. Fix a stage-1 design (signatures S_i) and outcome W. Every positive set P with union of signatures W is a member of the class
"W". Stage-2 tests see only which pools meet P. Strictness means: any P with |P|<=2 has a transcript shared with no other set (of any size)
of the same class. Corollary 5 / Theorem 1 of the paper give exactly: for each candidate set C (|C|<=2, union W; the empty set only for
W=0) and each v not in C that can coexist with C in the class (S_v subset of W) there is a pool containing v and missing C. Cases:
* no candidate set at W: nothing to do, r(W)=0 (|P|>2 with union W is then declared "more than 2" for free, and P of size <=2 cannot produce W);
* P a strict superset of a candidate set C: P contains some v not in C with S_v subset of W, so the (C,v) constraint separates it;
* P of size >=3 containing no candidate set but whose transcript equals that of a candidate set C': any x in P\C' has S_x subset of W
  and a pool containing x and missing C', so the transcripts differ. So (C,v) over all pairs suffices (this is the converse direction of
  the paper's proof of Theorem 1);
* dummy elements (items with S_v subset of W lying in no candidate set) must still appear in a pool missing each C; giving them the
  all-ones code is WLOG because a dummy only occurs in the role v. Both build() (gt.c, members get the CSP, others get `full`, and the
  constraint (a,b,v) is generated for every v in E) and build_sets()+build_graph() (gt2p.c: vertices (C,v) for all elems) do this;
* empty set only at W=0 (csa=csb=-1 only if W==0), singletons need S=W, pairs need union W: exactly as in both files.

Machine check (my own, from the definition, not from the theorem): `semanticQ` (core.hpp) brute-forces the minimum number of pools for a
class directly by enumerating all pool tuples and all subsets P of E with union W and comparing transcripts (candidate of size <=2 versus
any other set). On 400 random small designs (2871 outcome cases, r from 0 to >3) it agreed with my closed-pool set-cover solver, with
`gt.c: build+feasible` (wrapper wrap1) and `gt2p.c: check_W` (wrap2) in every case (cmp_a.py: "mismatches 0"). A second batch of 1500
random designs with k<=5, n<=16 (about 20k outcomes, q=0..4): wrap1 == wrap2 on all of them, and my solver agrees on the 1162 outcome cases it
finished before I stopped it (0 mismatches). The full-simulation checker has a negative control (dropping a stage-2 pool of a valid design
is detected as non-strict).

## 2. Coverage (PASS)

A 2-stage strategy with worst case <=9 tests has k stage-1 pools (a fixed number, nonadaptive) and, for each outcome W, a nonadaptive
stage 2 of at most 9-k tests; padding to exactly q=9-k pools per outcome only helps the model, and different stage-2 sizes per outcome are
handled by the max. Stage-1 pools that are empty or identical are just signature bits that are never set / always equal; items in no pool have
signature 0 (allowed, they occur in the known n=18 design). The exhaustive search never assumes pools are nonempty or distinct (k=5, no pool
bounds); for k>=6,7,8 emptiness/small pools are excluded by Lemma A (|Q_j|>=6), and for k<=4 by Lemma A + B. k=0 is a single nonadaptive stage
of 9 tests, identical to k=9,q=0 (the same set of strategies), so k=0 is not needed separately. Cases k=1..9 are exhaustive.

## 3. Lemmas

### 3a Lemma A (PASS)
Proof re-derived. Take N subset of [k], I_N = items avoiding all pools of N. Run the original strategy on I_N, simulating the pools of N as
empty: stage-1 outcomes are the original ones with zeros on N, stage-2 pools are intersected with I_N. Transcripts coincide with those of the
original strategy for every P within I_N, so the restricted strategy is strict, uses (k-|N|) + q = T-|N| tests, and hence |I_N|<=f(T-|N|),
where f is well defined by monotonicity in n. For W: N=[k]\W gives |N|=k-|W| and T-|N|=q+|W|. For a pool: N={j}, T-1=8, so n-|Q_j|<=f(8)=13
and |Q_j|>=6. For two pools: n-|Qi u Qj|<=f(7)=9, |Qi n Qj|<=|Qi|+|Qj|-10.

### 3b Lemma B (PASS)
Proof re-derived, no gap. If P != P' are 2-sets of the same class W with v(P) subset of v(P'), take x in P\P'. Then P''=P' u {x} has |P''|=3,
signature union W u S_x = W (because x is in P), and every stage-2 pool containing x meets P, hence meets P'. So P', P'' have identical
transcripts, contradicting strictness. Antichain, so <= C(q,floor(q/2)) pairs per W (Sperner). Pairs hitting Q_j: a pair {x,y} with only x in Q_j
has union of signatures containing S_x, which contains j, so its W contains j; W is the union, not the intersection. So "pairs hitting pool
j" lie in the 2^(k-1) outcomes containing j, and the count C(19,2)-C(19-a,2) <= 2^(k-1) C(q,floor(q/2)) is right.
(Note that the per-W pair bound used as a search prune is logically implied by r(W)<=q, so it is only a speed-up; the pool-window
consequences are the real use of the lemma.)

### 3c Arithmetic (PASS)
H(a)=171-C(19-a,2): H(6)=93, H(7)=105, H(9)=126, H(10)=135 (recomputed). Bounds 2^(k-1)C(q,floor(q/2)): k=1:70, k=2:70, k=3:80, k=4:80,
k=5:96, k=6:96, k=7:128, k=8:128. With a>=6 (H>=93): k=1..4 impossible (93>80), k=6: a<=6, so a=6 exactly (96>=93, 105>96), k=7,8: a<=9
(126<=128<135), so 6..9. INTERCAP for k=6: |Qi n Qj| <= 6+6-10 = 2 (Lemma A with N={i,j}, f(7)=9). All correct. (k=5 has the same bound
a<=6 but the search for k=5 was run without pool assumptions, which is safer.)

### 3d f-values (PASS / GAP)
* f(0..6)=0..6: re-derived by my own exhaustive search (mysearch, no f-prune, no Sperner prune, no top-pool prune): for each T=1..6
  and every k=1..T there is no design for n=T+1; individual testing gives n=T. (Output in the audit session; e.g. T=6: n=7, k=1..6, 0 designs.)
* f(7)=9: t(9,2,2)=7 and t(10,2,2)=8 are published results of the paper (appendix); my own run reproduces the n=9, 7-test design (k=4, q=3:
  0 0 0 3 5 6 9 10 12), but I did not complete an independent n=10, T=7 impossibility run for small k (my CSP is too slow for k=1,2 with q=5,6).
  GAP in the sense of "taken from the paper". It is only used to reach INTERCAP (k=6) and FT[7]; the larger f is the safer direction,
  and k=7/8 do not need it for pool windows... (they use FT[7]=9 only as a speed prune).
* f(8)=13 depends on t(14,2,2)=9 (the note's Theorem 1, proved by the same machinery). I re-ran n=14, T=8 with my own code (no f/Sperner/top
  prunes, pools 0..14): k=8 (q=0) 6,115,330 nodes, k=7 (q=1) 32,512,744, k=6 (q=2) 10,915,559, k=5 (q=3) 334,880: all 0 designs and
  node counts identical to the note's table. (k=2,3,4 runs with my CSP did not finish in the time box and are NOT independently reproduced; k=1 is the hand argument, which I checked: s<=6 negative gives
  >=8 left items and t(8,2,1)=8; s>=7 positive gives >=70 antichain pairs >64, Lemma 11, 8 more tests.)

## 4. Prunes and symmetries (PASS, with two minor defects)

For every implementation, the actual prunes (read from code):

exh2.c (first impl., k=5): (i) sorted signature sequence; (ii) singleton-count ordering cntsig[2^j]<=cntsig[2^(j-1)]; (iii) pool size bounds
(here 0..19, none); (iv) candidate-set count per W <= 2^q (ccnt, includes empty set at W=0); (v) exact CSP r(W)<=q for all W containing the new
signature. gt.c CSP: forward-checking propagation, MRV dynamic variable order, value-precedence symmetry (h&(h+1)), non-members preassigned
all-ones, node limit 2e9 (see below).
exh5/6/7.c: adds (vi) pcnt: pairs with union W <= C(q,q/2) (Lemma B, implied by (v)); (vii) mW[W] <= f(q+|W|) (Lemma A); (viii) pool bounds
minp..maxp; (ix) exh6/7: INTERCAP on pairwise pool intersections; (x) exh7: top-pool rule idx>=n-maxp for the first item containing pool k-1.
gt2p.c: same (i),(ii),(iv),(vi),(vii),(viii),(x) (flags usef,usepair,usetop), exact DSATUR colouring with "next unused colour" symmetry.
cf.c: nondecreasing, distinct signatures strictly increasing (equal signatures are covered), singleton ordering, direct 2-CFF check.

Completeness proofs.
* Monotonicity: adding an item adds elements to E, candidate sets and (C,v) constraints for each W containing its signature, never removes
  any; a feasible assignment/colouring of the larger instance restricts to the smaller. So ccnt, pcnt, mW and "r(W)<=q" are monotone: a failure
  at a prefix is a failure for all completions. Only W containing the new signature change (all code loops over exactly those W). OK.
* Singleton ordering: permuting pools preserves the cost; sort pools by singleton count (descending); then the sorted multiset is enumerated
  once; when the first item with signature 2^j is placed, the count of 2^(j-1) is final (smaller values come first). OK and compatible with the
  uniform pool-size bounds (symmetric in pools) and with R3/R4.
* Top-pool rule: after sorting by value, all items containing the top bit come last and number |Q_{k-1}| <= maxp, so the first such item
  has index >= n-maxp. OK (only valid under the pool bound, which is proven for k=6,7,8; for k=5 maxp=19 makes it vacuous).
* INTERCAP=2 (k=6): Lemma A with f(7)=9 and sizes exactly 6, proven above; the repo also ran exh6 INTERCAP=2 without top rule (121M nodes) and
  the weaker pool window 6..9 without INTERCAP/Sperner (433,804 nodes): all 0. A run without INTERCAP is also in the repo
  (n19_k6_p6-9_noINTERCAP, and gt2p k=6 288M nodes without it).
* CSP value symmetry with dynamic variable ordering: valid. The propagation, the domains and the MRV choice (smallest popcount, ties by index)
  are all equivariant under permutations of the q pool labels, so for any solution one can build a relabelling incrementally along the
  search path in which each newly used pool label is the lowest unused one; hence some solution survives the h&(h+1) filter. The non-member
  all-ones code is invariant under relabelling. DSATUR with first-unused colour is the standard colour-symmetry argument.
* Dummies as all-ones: WLOG (only appear in role v).
Flags (none affects the conclusion):
  - gt.c `bt2`/`btold` abort with "infeasible" when `++nodes>nodelimit` (2e9). Unreachable in practice (calls are microseconds; the two
    other solvers agree), but formally an unsound failure mode of exh*.
  - Only the f-values and the cap on candidate sets rely on outside results (3d).

## 5. k=9 (PASS)
Strict nonadaptive d=2 is exactly 2-cover-free: r(W)=0 for all W means no (C,v) pairs, i.e. no item with signature contained in the union of
the signatures of two others (or one other, or the empty set), derived directly from build(). Table 1 of the paper gives N1(T=9)=12.
My own exhaustive search (mycff.cpp, different ordering, symmetry = smallest member WLOG {0..s-1}, sorted by (size,mask)): t=5..8 points: N=t
possible, N=t+1 not (matches N1(8)=8); t=9: a family of 12 exists (STS(9)), N=13 impossible after 29,943,420 nodes. This agrees with cf.c.
Since 13 < 19, k=9 is excluded; k=0 identical.

## 6. Spot checks
(a) Done, see 1: 0 mismatches between my semantic brute force, my cover solver, gt.c and gt2p.c.
(b) f(0..6) re-derived (3d); n=14, T=8 re-derived for k=5..8 (3d). f(7) GAP.
(c) The exhaustive searches (exh2/exh7, pools 0..18 or 5..9) find exactly ONE n=18 design for k=6,q=3 up to symmetry
   (0 0 0 3 5 6 9 10 12 17 18 20 24 33 34 36 40 48; the other k did not finish in my time box), so "three random valid n=18 designs" are three random
   pool-permutation/item-shuffle relabelings of it. All three: verify_full finds stage-2 pools with my solver (max 3) and the full simulation
   over all 2^18 = 262,144 sets P (not only |P|<=3) gives 320 transcript classes, 0 violations, total tests 6+3=9 => STRICT.
(d) For the known design and each of the three relabelings, adding a 19th item of each of the 64 possible signatures: 0 of 64 feasible
   (58 fail because some outcome has more than 2^3=8 candidate sets, 6 fail by exact cover with 3 pools). So the known n=18 design does not extend.

## 7. Independent re-run of the n=19 search (own mysearch.cpp)
Own solver (candidate-set count cap, intersection-form exact solver for q<=3: choose <=q sets A_i, unions of candidate sets, with every
candidate C equal to the intersection of the A_i containing it; this is Cor. 5 with pool = E \ A_i), same enumeration idea, all prunes
R0-R4 re-implemented by me:
| k | q | pools | my nodes | their nodes | designs |
|---|---|-------|----------|-------------|---------|
| 6 | 3 | 6..6, INTERCAP 2 | 226,207 | exh7 226,207 | 0 |
| 7 | 2 | 6..9 | 13,504,468 | exh7 and gt2p 13,504,468 | 0 |
| 8 | 1 | 6..9 | 32,534,044 | gt2p 32,534,044; exh7 as re-run today 32,534,044 | 0 |
| 5 | 4 | 0..19 | see below | exh2/gt2p 4,288,517 | - |
k=5 (q=4): NOT independently re-run by me (my plain CSP for q=4 did not finish within the time box; n=19 run killed at ~28 min).

## D. Documentation defect (FAIL, minor)
docs/results19.md, docs/note-draft.md Sec. 4 and the logs state exh7 k=8: 12,399,312 (log: 12,449,799) nodes and explain the difference to gt2p
(32,534,044) by "different prune sets", and the note's caveat says only k=5 and k=7 counts agree. Re-running the current exh7.c (same
arguments) I obtain 32,534,044 nodes and 195,093,019 CSP calls, 0 designs, identical to gt2p and to my independent program. So the
stored k=8 log was produced by an older exh7 version, the explanation in the text is wrong, and in fact all implementations agree for
k=5,7,8 (and exh7 agrees with mine for k=6). The older, smaller tree is not needed for the proof. The text should be corrected.
Also: README/note should state that the n=14 hand case and Lemma A use f(8)=13 from the same computer search (circular only in the sense of
the same method, but I reproduced it).

## Final verdict
The claim t(19,2,2)=10 (no 9-test 2-stage strict strategy for n=19) is SUPPORTED. The reduction (cost = k + max r(W)) is exact (checked
against a definition-level brute force), the case split is exhaustive, Lemmas A and B and the pool windows are correct, every prune and
symmetry is justified, and my independent program reproduces the exhaustive results for k=6,7,8 and the n=14 numbers that feed f(8)=13;
k=9 is confirmed by an independent 2-CFF search and by the literature value 12. Remaining gaps: (1) f(7)=9 is taken from the paper (t(10,2,2)=8)
and not re-derived; (2) f(8)=13 rests on the computer proof of t(14,2,2)=9 (re-run by me for k=5..8 only; k=2..4 not reproduced by me, k=1 by hand);
(3) the stored k=8 node count / "different prune set" explanation is stale (documentation fix); (4) my own re-run of k=5 (n=19) did not finish, so k=5 rests on exh2 and gt2p, which agree exactly (4,288,517 nodes, 0 designs).
