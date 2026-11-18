> **Current preprint clarification (18 November 2026):** Historical text below retains its original nonadaptive-capacity argument. In the current manuscript, the n19 and n28/n29 nonadaptive endpoints follow directly from the pair-antichain bound; g(9)<=12 is not a premise. The n28/n29 theorem remains conditional on f(9)<=18. See manuscript.pdf and STATUS.md.

> Historical source document, retained for context. Current release status is in ../README.md. AI review only; no independent human review. Links may refer to the full private research repository, not this curated subset.

> **DRAFT — not submitted; novelty check incomplete (verify "cited by" lists before any submission).**
>
> Status as of 2026-10-19 (reviewer-driven revision of definitions, dependencies and wording). Labels used throughout: t(19,2,2)=10 is a *supported computer-assisted candidate; novelty provisional*; t(27,2,2), t(28,2,2), t(29,2,2) = 11 are *conditional candidates*. The second-solver cross-check for n=27 (gt2p) is complete and agrees with exh7: no design. Update 2026-10-21: t(26,2,2)=11 is a further conditional candidate (Section 5, n=26; the 17-item classification rests on exh7 only), so $f(10)\in[24,25]$.

# t(19,2,2)=10 by computer-assisted proof, and conditional consequences t(27,2,2)=t(28,2,2)=t(29,2,2)=11, with independent reproductions of t(14,2,2)=9 and t(20,2,2)=t(21,2,2)=10

**Steven Minh Phat Duong** — *[affiliation: TODO]*

## Abstract

In strict group testing with at most $d=2$ positives among $n$ items, the searcher must either identify the positive set exactly or report that there are more than $d$. Damaschke, Sheikh Muhammad and Wiener (J. Combin. Theory Ser. A 126 (2014) 70–91) determine $t(n,2,2)$, the minimum worst-case number of pooled tests in two stages, for many $n$, but leave $t(19,2,2)$ open (9 or 10). Grankvist [G] settled $t(14,2,2)=9$ and $t(20,2,2)=t(21,2,2)=10$ by hand. We show that no two-stage strategy with 9 tests exists for $n=19$, so $t(19,2,2)=10$ (the upper bound 10 is [DSW, Proposition 15]). This is a supported computer-assisted candidate; novelty is provisional (it is not peer reviewed, and no citation-index check was possible). Our search independently reproduces $t(14,2,2)=9$ and $t(20,2,2)=t(21,2,2)=10$, which are not new. If the $n=19$ result stands, [DSW] and [G] together determine $t(n,2,2)$ for every $n\le 24$ (see the Consequence paragraph in Section 4). Conditionally on the same result, counting arguments give $t(28,2,2)=t(29,2,2)=11$ by hand, and $t(27,2,2)=11$ with a negative search (Section 5; two independent implementations agree there is no design; these are negative-search logs, not independently checkable certificates). All three are conditional candidates. We also record $t(26,2,2)=11$ as a conditional candidate (Section 5; it rests on one implementation, exh7, for the 17-item classification, and on negative-search logs, not certificates). Hence, with the same condition, $f(10)\in[24,25]$, the largest $n$ that 10 tests can handle; $n=25$ is open. The proof is computer-assisted: the cases of few first-stage pools are settled by two short counting lemmas proved in full, and an exhaustive search over all multisets of first-stage pool signatures covers 2 to 8 first-stage pools (one pool is settled by hand). Two separately written second-stage solvers were run on the same enumeration; they agree on the verdict and on the total node count in the runs where they apply the same prunes. This is evidence of consistency, not a step-by-step equivalence check, and the two programs share the same enumeration and symmetry scheme by design. The same code reproduces the known values $t(9,2,2)=7$, $t(10,2,2)=8$, $t(13,2,2)=8$ and $t(15,2,2)=9$.

## 1. Model, definitions and notation

All definitions in this section are used unchanged in Sections 2 to 7.

### 1.1 The model

There are $n$ items $[n]$ and an unknown positive set $P\subseteq[n]$; $|P|$ may be anything. A *pool* is any subset $Q\subseteq[n]$ (sizes $0,\dots,n$ and arbitrary overlaps between pools are allowed: *unrestricted overlap*); testing $Q$ returns "positive" iff $Q\cap P\ne\emptyset$. A *two-stage deterministic strategy* consists of

* stage 1: a fixed list of $k\ge0$ pools $Q_1,\dots,Q_k$, tested together (nonadaptively);
* after the stage-1 outcome $W\subseteq[k]$ (the set of positive stage-1 pools), stage 2: a fixed list of $q(W)$ pools, determined by $W$ alone and tested together;
* an output, determined by the full transcript, which is either a set or the symbol "$>2$".

The strategy is *strict* (with $d=2$) if for every $P$ with $|P|\le2$ it outputs exactly $P$, and for every $P$ with $|P|>2$ it outputs "$>2$". Its *cost* is the worst-case number of tests, $k+\max_W q(W)$. Then
$$t(n,2,2)=\min\{\text{cost of a strict two-stage deterministic strategy for }n\text{ items}\}.$$
We write $T$ for the total number of tests, $k$ for the number of stage-1 pools and $q=T-k$ for the (padded) number of stage-2 pools; padding to exactly $q$ pools after every outcome only helps, so "cost $\le T$" is the same as "a strategy with $k$ stage-1 pools and at most $q=T-k$ stage-2 pools exists for some $k\in\{0,\dots,T\}$". The case $k=0$ is one nonadaptive stage and coincides with $k=T$, $q=0$. Restricting a strict strategy to a subset of the items gives a strict strategy, so $t(n,2,2)$ is nondecreasing in $n$. Hence
$$f(T)=\max\{n:\ t(n,2,2)\le T\}$$
is well defined.

### 1.2 Stage-1 signatures and outcomes

The *signature* of item $i$ is $S_i=\{j\in[k]: i\in Q_j\}\subseteq[k]$. A stage-1 design is, up to relabelling the items, a multiset of $n$ signatures. If the positive set is $P$, the outcome is $W=\bigcup_{i\in P}S_i$. For an outcome $W$:

* a *candidate set* is a $C\subseteq[n]$ with $|C|\le2$ and $\bigcup_{i\in C}S_i=W$ ($C=\emptyset$ is a candidate only for $W=\emptyset$); a *candidate element* is an item lying in some candidate set;
* $m_W=|\{i: S_i\subseteq W\}|$ is the number of items compatible with $W$;
* $p(W)$ is the number of 2-element candidate sets, i.e. of 2-sets $\{x,y\}$ with $S_x\cup S_y=W$;
* $r(W)$ is the minimum number of stage-2 pools after outcome $W$ that make the strategy strict on all $P$ with outcome $W$.

By [DSW] (Theorem 1 / Corollary 5, below), $r(W)$ is the set basis number of the complements of the candidate sets (Section 3(a)). A strategy with $k$ stage-1 pools and $q$ stage-2 pools exists iff some multiset of signatures has $r(W)\le q$ for all $W\subseteq[k]$.

### 1.3 Constants and the two lemmas

* $B_q=\binom{q}{\lfloor q/2\rfloor}$ and $H_n(a)=\binom n2-\binom{n-a}2$ (the number of 2-sets of items that meet a pool of $a$ items; we write $H(a)$ when $n$ is fixed). $\delta(W)=B_q-p(W)$ (the *deficiency*; we avoid the letter $d$, which is the defect parameter).
* $g(m)$ is the largest number of items for which $m$ tests in a single nonadaptive stage are strict for $d=2$. Equivalently, the largest size of a 2-cover-free family on $m$ points: nonempty sets, none contained in the union of two others. $g(9)=12$ ([DSW, Table 1, row $N_1$] and our `cf` search, Section 4).
* **Lemma A (negative-set restriction; general form, proved in Section 4).** For a strict strategy with total $T$ tests and a set $N\subseteq[k]$ of stage-1 pools, $|I_N|\le f(T-|N|)$ where $I_N=\{i:S_i\cap N=\emptyset\}$. Two special cases are used:
  * per-pool form ($N=\{j\}$): every stage-1 pool $Q_j$ of $a_j$ items has $n-a_j\le f(T-1)$;
  * per-outcome form ($N=[k]\setminus W$): $m_W\le f(q+|W|)$ for every $W\subseteq[k]$.

  (Taking $N=\{i,j\}$ gives $n-|Q_i\cup Q_j|\le f(T-2)$.)
* **Lemma B (Sperner bound; proved in Section 4).** For a strict strategy and a stage-1 outcome $W$ followed by $q$ stage-2 pools, $p(W)\le B_q$ (per-outcome form). Consequently every stage-1 pool of $a$ items satisfies $H_n(a)\le2^{k-1}B_q$ (per-pool form).

### 1.4 Results of [DSW] and [HH] that are used

* **Theorem 1 / Corollary 5 [DSW].** A nonadaptive pooling design (the last stage) is strict for a given candidate hypergraph iff the complement of every candidate set is a union of pools; equivalently, for every candidate set $C$ and candidate element $v\notin C$ some pool $Q$ has $v\in Q$ and $Q\cap C=\emptyset$. Hence the minimum number of last-stage pools is the minimum size of a set basis of the complements of the candidate sets.
* **Lemma 11 [DSW].** If $m>2^r$ candidate sets form an antichain, then strict group testing needs at least $r+2$ tests, even adaptively.
* **Proposition 15 [DSW].** For $m=5,6,7,8$ and $n\le\binom m2+3=13,18,24,31$ respectively, $t(n,2,2)\le 8,9,10,11$ (upper bounds, by explicit designs).
* $t(8,2,1)=8$ (implicit in Huang and Hwang, SIAM J. Discrete Math. 14 (2001) 540–548, as noted in [DSW]).

Every later use of $t$, $f$, $g$, $S_i$, $W$, $m_W$, $p(W)$, $B_q$, $H_n(a)$, $\delta(W)$, $r(W)$, Lemma A and Lemma B refers to these definitions.

## 2. Results

**Proposition 1 (reproduction).** $t(14,2,2)=9$. Grankvist [G] proved this by hand; we reproduce it by computer in Section 3. Not new.

**Theorem 2 (supported computer-assisted candidate; novelty provisional).** $t(19,2,2)=10$. [G, section 5.1, p.24] lists $n=19$ as open. The proof is in Section 4; what it rests on is listed in Section 6.

**Proposition 3 (reproduction).** $t(20,2,2)=t(21,2,2)=10$. Grankvist [G, section 5.4] proves these by hand; they also follow from Theorem 2 by monotonicity. Not new.

The upper bound $t(14,2,2)\le t(15,2,2)=9$ and the lower bound $t(14,2,2)\ge t(13,2,2)=8$ are in [DSW]. We prove in Section 3 that $8$ tests do not suffice (the hand proof is in [G]). For Theorem 2 and Proposition 3, $t(n,2,2)\le t(22,2,2)=10$ for $n\le 21$ is [DSW] (Proposition 15 with $m=7$ gives $t(n,2,2)\le10$ for all $n\le\binom72+3=24$), and the lower bound is proved in Section 4: no 9-test strategy exists for $n=19$; by monotonicity (restricting a strategy to a subset of the items gives a strategy) $t(20,2,2),t(21,2,2)\ge t(19,2,2)\ge10$.

## 3. Proof that 8 tests do not suffice (Grankvist's result, reproduced by computer)

Suppose a two-stage strict strategy uses at most 8 tests. Let $k$ be the number of stage-1 pools, so stage 2 has at most $q=8-k$ pools for every stage-1 outcome. Here $0\le k\le 8$; $k=0$ is purely nonadaptive and is the same as $k=8$.

### (a) Reduction to signatures

The *signature* of item $i$ is $S_i\subseteq\{1,\dots,k\}$, the set of stage-1 pools containing it. A stage-1 design is, up to relabelling items, a multiset of $14$ signatures. If the positive set is $C$, the set of positive pools is $W=\bigcup_{i\in C}S_i$. So for an outcome $W\subseteq[k]$ the candidate elements are the items with $S_i\subseteq W$, and the candidate sets are the $C$ with $|C|\le 2$ and $\bigcup_{i\in C}S_i=W$ (including $C=\emptyset$ when $W=\emptyset$). By Corollary 5, the stage-2 cost for outcome $W$ is
$$r(W)=\min\{\,q' : \text{there are } q' \text{ pools such that for all candidate } C,\ v\notin C \text{ some pool contains } v \text{ and misses } C\,\},$$
which is the set basis number of the complements of the candidate sets. The cost of the design is $k+\max_W r(W)$, and the strategy exists iff some multiset has $\max_W r(W)\le q$.

$r(W)\le q$ is checked in two equivalent ways. (i) Constraint satisfaction: assign each candidate element $v$ a $q$-bit code $a(v)$ (membership in the $q$ pools), subject to $a(v)\not\subseteq a(c_1)\cup a(c_2)$ for every candidate set $C=\{c_1,c_2\}$ (similarly for $|C|\le1$) and $v\notin C$. Elements in no candidate set get the all-ones code, which is without loss since a larger code only helps. (ii) Graph colouring: the vertices are pairs $(C,v)$ with $v\notin C$, and $(C,v)\sim(C',v')$ iff $v\in C'$ or $v'\in C$; two pairs may share a pool exactly when they are non-adjacent, so $r(W)$ is the chromatic number of this conflict graph. Also, distinct candidate sets need distinct unions of pools, so $r(W)\le q$ forces at most $2^q$ candidate sets for $W$.

### (b) Monotone pruning

Items are added one at a time. Adding an item can only add candidate elements, candidate sets and constraints $(C,v)$ for each $W$; existing ones remain. A feasible assignment for the larger instance restricts to a feasible assignment for the smaller one. Hence, for every $W$, both "number of candidate sets $\le 2^q$" and "$r(W)\le q$" are monotone: if either fails on a partial design, it fails on every completion. The search discards a partial design as soon as a violation occurs for some $W\supseteq S_{\text{new}}$ (the only outcomes whose candidate structure changed). The prune is therefore valid, and the search is exhaustive over completions. No assumption on pool sizes (sizes $0,\dots,14$ allowed) is used.

### (c) Symmetry

Permuting the $k$ stage-1 pools relabels signatures but preserves the cost. Every multiset of signatures can be brought, by such a permutation, to one in which the numbers $c_j$ of items with singleton signature $\{j\}$ satisfy $c_1\ge c_2\ge\dots\ge c_k$. The search enumerates multisets as sequences of signatures (encoded as bitmasks) in nondecreasing order, so each multiset is generated once, and rejects a partial sequence if adding a singleton $\{j\}$ would make $c_j>c_{j-1}$. This is a valid prefix test: in nondecreasing order all items with signature $2^{j-1}$ precede those with signature $2^j$, so $c_{j-1}$ is final when the first $\{j\}$ item is considered. Every multiset has a representative satisfying the constraint, and it is enumerated in sorted order, so nothing is lost. (The second solver additionally breaks the symmetry of the second-stage pools in the CSP/colouring by the usual first-use ordering of colours; this is also valid.)

### (d) $k=1$ by hand

Let the single stage-1 pool have $s$ items; stage 2 has 7 tests, all nonadaptive.

* $s\le 6$ (including $s=0$) and the pool is negative: at least $14-s\ge 8$ items remain, still with up to 2 positives and one stage left. Since $t(8,2,1)=8$ and $t(n,2,1)$ is nondecreasing in $n$, at least 8 further tests are needed; total $\ge 9$.
* $s\ge 7$ and the pool is positive: every 2-set meeting the pool is a candidate. There are $\binom{14}{2}-\binom{14-s}{2}\ge 91-21=70$ such 2-sets, an antichain of size $>2^6=64$. By Lemma 11 (with $r=6$) at least $r+2=8$ more tests are needed; total $\ge 9$.

So $k=1$ cannot achieve 8 tests. (The program also finds none for $k=1$, in 42 search nodes with both solvers.)

### (e) Computation for $k=2,\dots,8$

For each $k$ we ran the search of (a)–(c) with $q=8-k$ and $n=14$ over all pool sizes. Results (both implementations report the same total node count in every row, and 0 designs found in every row):

| $k$ | $q$ | search nodes |
|---|---|---|
| 2 | 6 | 219 |
| 3 | 5 | 1,406 |
| 4 | 4 | 17,685 |
| 5 | 3 | 334,880 |
| 6 | 2 | 10,915,559 |
| 7 | 1 | 32,512,744 |
| 8 | 0 | 6,115,330 |

(For $k=1$: 42 nodes. In the first implementation $k=2..8$ also report 372; 4,037; 104,555; 3,971,758; 193,046,962; 824,426,195; 291,677,463 exact-$r(W)$ calls.)

Two implementations were used, with different second-stage solvers but the same enumeration scheme, symmetry rule and candidate-set cap (so they are not independent in those respects):

1. a constraint-propagation CSP over pool-membership codes with forward checking and an MRV heuristic (`gt.c`, `exh2.c`);
2. an exact DSATUR backtracking colouring of the conflict graph, written without sight of the first (`independent/gt2.c`).

Both implementations prune on the predicate "$r(W)\le q$", and they agree on the verdict and on the total node count in every row. This is evidence of consistency, not a step-by-step equivalence check: equal totals do not show that the two programs visit the same nodes in the same order or return the same answer at each individual node, and the two programs share the same enumeration/symmetry scheme by design. What the totals do support is that the shared scheme is implemented the same way in both. Total effort was about 2 hours for the first implementation and under 15 minutes for the second, on 4 cores.

**Validation against known values.** The same code (i) finds designs where [DSW] gives upper bounds and finds none where [DSW] proves lower bounds: no design for $n=9$ with 6 tests, for $n=10$ with 7 tests and for $n=13$ with 7 tests (all $k$), while designs exist for $n=9$ with 7, $n=10$ with 8, $n=13$ with 8, $n=15$ with 9 ($k=5,q=4$) and $n=18$ with 9 ($k=6,q=3$), i.e. $t(9,2,2)=7$, $t(10,2,2)=8$, $t(13,2,2)=8$, $t(15,2,2)=9$; (ii) reproduces the Proposition 15 designs (costs 8, 9, 10 for $n=13, 18, 24$); (iii) agrees with an independent brute-force set-cover computation of $r(W)$ on 336 (design, $W$) cases; and (iv) found designs are verified by a standalone brute-force checker over all $2^n$ defective sets.

**Remark.** The same machinery, with additional pruning justified in Section 4, settles $n=19$ with 9 tests.


## 4. Proof that $t(19,2,2)=10$, hence $t(20,2,2)=t(21,2,2)=10$

Suppose a two-stage strict strategy for $n=19$ uses at most $T=9$ tests, with $k$ stage-1 pools and at most $q=9-k$ stage-2 pools for every stage-1 outcome, $0\le k\le 9$ (Section 1.1). Signatures $S_i$, outcomes $W$, $m_W$, $p(W)$, $r(W)$, $f$, $B_q$ and $H_n(a)$ are as in Section 1; $r(W)$ is computed as in Section 3(a). The values used are $f(T')=0,1,2,3,4,5,6,9,13$ for $T'=0,\dots,8$. Here $f(7)=9$ comes from $t(9,2,2)=7$ and $t(10,2,2)=8$ [DSW], and $f(8)=13$ from $t(13,2,2)=8$ [DSW] and $t(14,2,2)=9$ (Proposition 1; proved by hand in [G], reproduced by our search). The values $f(1),\dots,f(6)$ were obtained with our search code (and $f(T')\ge T'$ by individual testing); a separately written program of the independent audit re-derived them (`docs/AUDIT-n19.md`, 3d). See the dependency chain in Section 6.

### Lemma A (negative-set restriction)

*Let $N\subseteq[k]$ be a set of stage-1 pools and $I_N=\{i: S_i\cap N=\emptyset\}$. Then $|I_N|\le f(T-|N|)$. In particular (per-outcome form), for $W\subseteq[k]$ the number $m_W$ of items with $S_i\subseteq W$ satisfies $m_W\le f(q+|W|)$, and (per-pool form) every stage-1 pool $Q_j$ satisfies $n-|Q_j|\le f(T-1)$.* (This is the statement of Section 1.3 for a general $T$.)

*Proof.* Consider the instance whose items are $I_N$ (at most 2 of them positive). Run the given strategy with the stage-1 pools $Q_j\cap I_N$, $j\notin N$, and, for each stage-1 outcome $W\subseteq[k]\setminus N$, the stage-2 pools of the original strategy for that outcome intersected with $I_N$. This is a two-stage strategy with $k-|N|$ stage-1 pools and at most $q$ stage-2 pools, hence at most $T-|N|$ tests. It is simulated faithfully: every pool $Q_j$, $j\in N$, is disjoint from $I_N$ and so is negative whatever the positive set inside $I_N$ is, so the original stage-1 outcome is the stage-1 outcome of the restricted strategy extended by zeros on $N$, and in stage 2 a pool's response depends only on its intersection with the positive set, which lies in $I_N$. Hence the restricted run reaches the same transcript as the original run, and its verdict is correct: if $|P|\le 2$ with $P\subseteq I_N$ the original strategy identifies $P$ exactly, and if $|P|>2$ with $P\subseteq I_N$ it reports "more than 2". So the restricted strategy is strict for $|I_N|$ items, i.e. $t(|I_N|,2,2)\le T-|N|$, which is $|I_N|\le f(T-|N|)$. For the second claim take $N=[k]\setminus W$: then $|N|=k-|W|$, $T-|N|=q+|W|$, and $I_N=\{i:S_i\subseteq W\}$. For the third take $N=\{j\}$. $\square$

For $n=19$ and $T=9$, $f(8)=13$ gives $|Q_j|\ge 19-13=6$ for every stage-1 pool. (Within the $n=19$ proof this is the only place where Proposition 1 is used; Section 5 uses $f(8)=13$ again for $n=18$. It is applied with all $k\ge1$, including pools of size 0.) Taking $N=\{i,j\}$ and $f(7)=9$ gives $19-|Q_i\cup Q_j|\le 9$, i.e. $|Q_i\cap Q_j|\le |Q_i|+|Q_j|-10$.

### Lemma B (Sperner bound)

*Fix a stage-1 outcome $W$ and let $q$ be the number of stage-2 pools used after it. (Per-outcome form: $p(W)\le B_q$.) For a 2-set $P$ of items with $\bigcup_{i\in P}S_i=W$ let $v(P)\subseteq\{1,\dots,q\}$ be the set of stage-2 pools meeting $P$. Then the sets $v(P)$ for distinct such $P$ are pairwise incomparable under inclusion; hence $p(W)\le B_q=\binom{q}{\lfloor q/2\rfloor}$. Consequently (per-pool form), if a stage-1 pool $Q_j$ has $a$ items, then*
$$H_n(a)=\binom n2-\binom{n-a}2\;\le\;2^{k-1}B_q.$$

*Proof.* Suppose $P\ne P'$ are 2-sets with union of signatures $W$ and $v(P)\subseteq v(P')$. Since $|P|=|P'|=2$ and $P\ne P'$, there is $x\in P\setminus P'$. Put $P''=P'\cup\{x\}$, so $|P''|=3>d$. Stage 1: $S_x\subseteq W$ because $x\in P$, so the positive pools for $P''$ are $W\cup S_x=W=$ those for $P'$. Stage 2 uses the same pools as both have outcome $W$. A stage-2 pool meets $P''$ iff it meets $P'$ or contains $x$; if it contains $x$ it meets $P$, so by $v(P)\subseteq v(P')$ it meets $P'$. Thus $v(P'')=v(P')$ and $P'$ and $P''$ produce identical transcripts. But a strict strategy must identify $P'$ exactly, and report "more than 2" for $P''$; a deterministic strategy cannot do both. So the sets $v(P)$ form an antichain in $2^{[q]}$ (in particular they are distinct, since equal sets are comparable), and by Sperner's theorem there are at most $\binom q{\lfloor q/2\rfloor}$ of them.

For the consequence: the 2-sets of items meeting $Q_j$ number $\binom n2-\binom{n-a}2$. Each has $j\in W$ for its outcome $W$, and there are $2^{k-1}$ sets $W\subseteq[k]$ containing $j$. Each such $W$ accommodates at most $\binom q{\lfloor q/2\rfloor}$ 2-sets. $\square$

The same lemma gives the per-outcome bound $p(W)\le B_q$ (used as a search prune). It is not a consequence of the bound $2^q$ on all candidate sets used in Section 3. As a sanity check (not part of the proof), the inequality of Lemma B had no violation on more than 16,000 exhaustively generated designs (`search/lemma_check.py`).

### Analytic exclusion of $k=1,2,3,4$

For $n=19$ let $H(a)=\binom{19}2-\binom{19-a}2$, which is increasing in $a$. Lemma A forces $a\ge6$ for every stage-1 pool, so every pool satisfies $H(a)\ge H(6)=171-78=93$. Lemma B requires $H(a)\le 2^{k-1}\binom{q}{\lfloor q/2\rfloor}$:

| $k$ | $q$ | $2^{k-1}\binom q{\lfloor q/2\rfloor}$ | needed $\ge H(6)=93$ |
|---|---|---|---|
| 1 | 8 | $1\cdot70=70$ | no |
| 2 | 7 | $2\cdot35=70$ | no |
| 3 | 6 | $4\cdot20=80$ | no |
| 4 | 5 | $8\cdot10=80$ | no |

Since $k\ge1$ there is at least one pool, and it violates Lemma B. So $k\le4$ is impossible. (Cross-check without Lemma B, only Lemma A and the Section 3 prunes: $k=4$ exhaustive search, 105,715 nodes, no design. $k=2,3$ cross-checks were started and stopped unfinished, so for $k=1,2,3$ the exclusion rests on the two lemmas alone.)

### Pool-size windows for $k=5,\dots,8$

Lemma B also gives upper bounds on pool sizes: $H(a)\le2^{k-1}\binom q{\lfloor q/2\rfloor}$ with $H(6)=93$, $H(7)=105$, $H(9)=126$, $H(10)=135$:

* $k=6$, $q=3$: bound $32\cdot3=96$, so $a\le6$; with $a\ge6$ every stage-1 pool has exactly 6 items. Moreover, by Lemma A with $f(7)=9$, $|Q_i\cap Q_j|\le 6+6-10=2$ for $i\ne j$.
* $k=7$, $q=2$ and $k=8$, $q=1$: bound $64\cdot2=128$, resp. $128\cdot1=128$, so $a\le9$ ($H(9)=126\le128<135=H(10)$); pools have $6\le a\le9$ items.
* $k=5$ ($q=4$): the search was run with no assumption on pool sizes.

Further search-time prunes: the counting bound $m_W\le f(q+|W|)$ of Lemma A, the bound on 2-sets per $W$ from Lemma B, and an ordering prune ("top pool": items are enumerated in nondecreasing signature order, so items of the last pool come last and, as that pool has at most the maximum allowed size, the first such item can only appear after at least $n-\text{maxsize}$ items). All prunes are monotone in adding items or are derived constants, as in Section 3(b).

### Exhaustive search for $k=5,\dots,8$

The search of Section 3 (multisets of 19 signatures in nondecreasing order, singleton-count symmetry, candidate-set cap $2^q$, exact $r(W)\le q$ for each $W\supseteq S_{\rm new}$) was run with the prunes above (`search/exh2.c`, `exh5.c`, `exh6.c`, `exh7.c`; second solver `independent/gt2p.c`). All runs found 0 designs.

| $k$ | $q$ | pool sizes | first implementation (CSP) nodes | second implementation (colouring) nodes |
|---|---|---|---|---|
| 5 | 4 | 0..19 | 4,288,517 (`exh2`) | 4,288,517 (157 s) |
| 6 | 3 | `exh7`: 6 (exactly); `gt2p`: no pool-size bound | 226,207 (`exh7`, with the intersection bound and top-pool prune) | 288,383,544 (5,362 s; no pool-size bound, stored log `independent/logs/n19full/k6.log`); with pools 6..6, 333,357 (re-run 2026-10-19) |
| 7 | 2 | 6..9 | 13,504,468 (`exh7`) | 13,504,468 (308 s) |
| 8 | 1 | 6..9 | 32,534,044 (`exh7`) | 32,534,044 (811 s) |

For $k=5$, $k=7$ and $k=8$ the two implementations report the same total node count and both find no design. As in Section 3, this is evidence of consistency, not a step-by-step equivalence check; the two programs share the enumeration/symmetry scheme and the prune definitions by design. (An earlier stored log gave 12,399,312 nodes for `exh7` at $k=8$; that log came from an older version of `exh7`, and the current code gives 32,534,044.) For $k=6$ the counts differ because the two runs use different prune sets and pool-size ranges (the intersection bound $|Q_i\cap Q_j|\le2$ and the top-pool prune are used by `exh7` only), so no node-count comparison is made there; both complete the search with no design. For $k=6$ we also ran `exh6` with the intersection bound but without the top-pool prune (121,237,245 nodes) and `exh7` with only $6\le a\le9$ and no intersection bound and no Lemma B per-outcome bound (433,804 nodes): 0 designs each. A third, separately written search by the independent audit (`docs/AUDIT-n19.md`, section 7) reports the same totals for $k=6,7,8$; it was written from the same prune list.

The machinery was re-validated on this setting: `exh6/exh7` still find designs for $n=13$ ($k=5$, $q=3$), $n=15$ ($k=5$, $q=4$) and $n=18$ ($k=6$, $q=3$, with the pool bounds), and reproduce the $n=14$ node counts of Section 3 for $k=5,6$.

### $k=9$ (nonadaptive)

With $q=0$ the 9 tests are a single stage, and strictness is equivalent to the pooled design being 2-cover-free (2-disjunct): no signature is contained in the union of two others, and no signature is empty or contained in another. By [DSW, Table 1] (row $N_1$, $T=9$, from the known one-stage results), 9 tests resolve at most 12 items for $d=2$. This agrees with a dedicated exhaustive solver (`search/cf.c`: 647,436,550 nodes, no family of 13 signatures; it finds 12, a Steiner triple system), which also agreed with `exh5` on 21 small $(k,n)$ cases, and with the audit's own search (`mycff`, no family of 13 after 29,943,420 nodes). The `cf 13 9` run was repeated after the 2026-10-19 hardening with the same result. Hence $g(9)=12$ and no 9-test nonadaptive strategy exists for $n=19$.

### Conclusion

No strategy with $T=9$ exists for $n=19$ for any $k\in\{1,\dots,9\}$ (and $k=0$ coincides with $k=9$), so $t(19,2,2)\ge10$. By monotonicity $t(20,2,2),t(21,2,2)\ge10$. With $t(21,2,2)\le t(22,2,2)=10$ from [DSW, Proposition 15] ($m=7$ gives $t(n,2,2)\le 10$ for $n\le 24$) we obtain Theorem 2 and Proposition 3.

**Consequence.** Combining Proposition 1, Theorem 2 and Proposition 3 with the values in [DSW] and [G], $t(n,2,2)$ is determined for every $n\le 24$ (in particular $t(n,2,2)=10$ for $19\le n\le 24$), assuming [DSW] and [G] list no other open $n\le24$ and that Theorem 2 holds; this should be re-verified against the papers before publication.

## 5. Consequences for n=27, 28 and 29 (conditional)

**Statement.** Assume Theorem 2, i.e. $t(19,2,2)=10$. Then $t(27,2,2)=t(28,2,2)=t(29,2,2)=11$. The upper bound is [DSW, Proposition 15] ($t(31,2,2)\le 11$ with $m=8$), and monotonicity gives $t(n,2,2)\le 11$ for $n\le 31$. The lower bounds below use Theorem 2 only through $f(9)\le 18$, i.e. $t(19,2,2)\ge 10$. The $n=27$ lower bound additionally uses a negative search (below) run with two implementations (exh7 and gt2p), which agree that there is no design. From the published values alone ($t(20,2,2)=10$ in [G]) the same counting gives no contradiction for these $n$.

**Notation.** Definitions are those of Section 1. Here $T=10$, with $k$ stage-1 pools and $q=10-k$ stage-2 pools, chosen after the full stage-1 outcome; $B=B_q$, $H(a)=H_n(a)$ with the $n$ of the case at hand, $p(W)$ as in Section 1.2, and the deficiency $\delta(W)=B-p(W)$. ($\delta$ is called $d(W)$ in `n28/REPORT.md`, `n29/REPORT.md` and the helper scripts.)

**Lemma A, per-pool form (Section 1.3, $T=10$).** Taking $N=\{j\}$: $n-a_j\le f(9)$. Here $f(9)=18$: $f(9)\ge18$ by $t(18,2,2)=9$ [DSW], and $f(9)\le18$ is exactly Theorem 2 ($t(19,2,2)\ge10$, hence $t(n,2,2)\ge10$ for $n\ge19$). So $a_j\ge n-18$ and pool $j$ meets at least $H(a_j)$ of the 2-sets. (This is the only place where Theorem 2 enters Section 5, apart from the use of $f(8)=13$ below.)

**Lemma B, per-outcome and per-pool forms (Section 1.3).** $p(W)\le B$ for every $W$, so $\delta(W)\ge0$, and each pool meets at most $2^{k-1}B$ of the 2-sets.

**$n=29$.** Lemma A gives $a_j\ge 11$, so $H\ge\binom{29}2-\binom{18}2=253$.

* $k=1,\dots,7$: $2^{k-1}B\le 192<253$. Excluded.
* $k=10$ (and $k=0$): the design is 2-cover-free. Take a point $p$; the sets avoiding $p$, and the sets containing $p$ with $p$ removed, are each 2-cover-free on 9 points. So there are at most $2g(9)=24<29$ items, where $g(9)=12$ (Section 1.3; computed by `cf 13 9`, 647,436,550 nodes, no family of 13, and listed in [DSW, Table 1]).
* $k=8,9$: here $2^{k-1}B=256$. For each pool $j$, the deficiencies $\delta(W)$ of the outcomes containing $j$ sum to $256-H(a_j)\le 3$. Summing over pools, $\sum_W |W|\,\delta(W)\le 3k$, i.e. at most 24 ($k=8$) or 27 ($k=9$). But $\sum_{W\ne\emptyset}\delta(W)\ge(2^k-1)B-\binom{29}2$, so $\sum_W|W|\,\delta(W)\ge 104$ ($k=8$) or $105$ ($k=9$). Contradiction.

**$n=28$.** Lemma A gives $a_j\ge 10$, so $H\ge\binom{28}2-\binom{18}2=225$.

* $k=1,\dots,7$: $2^{k-1}B\le 192<225$. Excluded.
* $k=10$ (and $k=0$): at most $2g(9)=24<28$ items, by the same argument.
* $k=8,9$: for each pool, the deficiencies sum to $256-H(a_j)\le 31$, so $\sum_W|W|\,\delta(W)\le 31k$, i.e. at most 248 ($k=8$) or 279 ($k=9$). The smallest weighted sum that achieves the required total deficiency is $8\cdot2\cdot1+28\cdot2\cdot2+30\cdot3=308$ for $k=8$ (required total 132), and $9+72+252+4\cdot4=349$ for $k=9$ (required total 133). Both exceed the upper bounds. Contradiction.

**$n=27$ (conditional on a negative search by two implementations).** Lemma A gives $a_j\ge 9$ for every pool. Suppose every pool has $a_j\ge 10$. Then $H\ge\binom{27}2-\binom{17}2=215$. For $k=1,\dots,7$ we have $2^{k-1}B\le 192<215$, and $k=10$ is excluded by $2g(9)=24<27$, as above. For $k=8,9$ the deficiency argument of the $n=28$ case gives $L>U$: $389>328$ ($k=8$, $q=2$) and $457>369$ ($k=9$, $q=1$), where $L$ is the lower bound on the weighted deficiency and $U$ its upper bound. So some pool $j$ has exactly $a_j=9$.

Take such a pool $j$. If it tests negative, the other 18 items, with the other $k-1$ stage-1 pools (restricted to these items) and the $q$ stage-2 tests of that branch, form a 9-test strategy for $n=18$: $(k',q)=(8,1)$ when $k=9$, and $(7,2)$ when $k=8$. Every pool of such a strategy has between 5 and 10 items, by Lemma A (with $f(8)=13$) and Lemma B. (The empty defective set makes every pool negative, so this case does occur.) An exhaustive search with `exh7` found no such strategy in either case (this is the only step that is a computer search). Both runs are COMPLETE with 0 designs (`n27/logs6h/`): for $(18;k'=8,q=1)$, 116,977,029 and 124,122,619 nodes in the two parts; for $(18;k'=7,q=2)$, 571,166,532 and 562,505,326 nodes. This contradiction gives $t(27,2,2)\ge 11$.

The step "$(a_j\ge 9)$ and the forced 9-pool" is hand counting, rechecked by `n27/force9_check.py`. The 18-item negative search is the only part that is a computer search. It was run with two implementations: `exh7` (Section 5 logs `n27/logs6h/`: 116,977,029 + 124,122,619 nodes for $(18;8,1)$, 571,166,532 + 562,505,326 nodes for $(18;7,2)$) and the separately written `gt2p` (no node limit, partitioned; `n27/run_gt2p_chunks.sh`, logs in `n27/logs_gt2p/`, `python3 -I n27/summarize_gt2p.py` reports OVERALL: COMPLETE). `gt2p` finished all 60 parts of the $(18;8,1)$ case (241,092,143 nodes) and all 150 parts of the $(18;7,2)$ case (1,133,670,027 nodes), every part STATUS COMPLETE, 0 designs. Two independent implementations agree there is no design. The node counts differ between the two solvers, as expected for different implementations, and are not claimed to match. The logs are negative-search logs, not independently checkable certificates, and since the programs share the enumeration/symmetry scheme the agreement is consistency evidence, not a step-by-step equivalence check. An earlier LP route (`n27/NOTES.md`) was feasible and did not settle $k=8,9$ on its own.

**$n=26$ (conditional candidate; one implementation for the 17-item part).** Candidate: $t(26,2,2)=11$, conditional on $t(19,2,2)=10$ and $g(9)=12$; the 17-item classification rests on one implementation (exh7) only, negative-search logs, not certificates; not peer reviewed; novelty provisional. Lemma A gives $a_j\ge 26-18=8$ for every pool, so $H(a_j)\ge H(8)=325-153=172$.

* $k=1,\dots,5$: $2^{k-1}B=126,140,140,160,160<172$. Excluded.
* $k=10$ (and $k=0$): at most $2g(9)=24<26$ items, as for $n=27$.
* $k=6,7$ ($2^{k-1}B=192$, so $8\le a_j\le 9$): if all pools had $a_j\ge9$ the deficiency argument would give $L>U$ ($70>18$, $91>21$), so some pool has exactly 8 items. If it tests negative, the other 18 items form a 9-test strategy $(18;k'=5,q=4)$ resp. $(18;6,3)$ with pools of 5 to 6 items (Lemma A with $f(8)=13$ and Lemma B). Exhaustive search: $(18;5,4)$ has no design; $(18;6,3)$ has exactly one, $D$ (it matches the unique $n=18$ design of [DSW, Proposition 15]); both solvers agree (`n26/logs/`). Extending $D$ by 8 items all lying in the new pool, with all pool sizes in $8..9$ and the $f(9)=18$ prune, gives no design (`n26/logs/ext_n26_k7_a8_D18.log`, exh7 only).
* $k=8,9$ ($q=2,1$): if all pools had $a_j\ge10$ then $L>U$ ($468>408$ for $k=8$, $561>459$ for $k=9$), so the smallest pool has 8 or 9 items. If a pool of 8 tests negative, the other 18 items form $(18;7,2)$ resp. $(18;8,1)$ with pools of 5 to 10 items: exactly the two shapes of the $n=27$ case, with 0 designs from exh7 and gt2p. If a pool of 9 tests negative, the other 17 items form $(17;7,2)$ resp. $(17;8,1)$; by Lemma A with $f(8)=13$ and Lemma B ($H_{17}(12)=126\le128<130=H_{17}(13)$) every pool has 4 to 12 items. The full exh7 classification of both shapes is COMPLETE with 0 designs: 200/200 parts each (partition by the depth-4 prefix index modulo 200), 23,453,318,160 nodes for $(17;7,2)$ and 27,571,051,900 nodes for $(17;8,1)$, about 112 CPU-hours (`n26/logs_full/`, `python3 -I n26/summarize_pilot.py n26/logs_full`). No second solver was run on the 17-item shapes.

So every $k$ is excluded and $t(26,2,2)\ge11$; with [DSW, Proposition 15] ($m=8$), $t(26,2,2)=11$. An adversarial audit of this chain (2026-10-21; arithmetic recomputed by script, log completeness and the partition scheme re-checked, `n26/NOTES.md`) found no gap. Inputs: $t(19,2,2)=10$ (through $f(9)=18$), $g(9)=12$, $f(8)=13$, $f(1),\dots,f(7)$, the $n=27$ 18-item runs, and the exh7 17-item and extension runs.

**Checks and dependencies.** The $n=28$ numbers were re-derived by a separate script (`n28/derive_independent.py`) and agree with the hand argument; the $n=29$ arithmetic is in `n29/count_bound.py`. The argument was found with AI assistance and checked by hand (`n28/REPORT.md`); it has not been checked by anyone else. For $n=28$ and $n=29$ the only computer input is $g(9)=12$ (the counting itself needs no search); for $n=27$ the inputs are $g(9)=12$ and the 18-item negative searches. The `cf 13 9` run was repeated after the 2026-10-19 hardening (COMPLETE, 647,436,550 nodes, no family of 13; `docs/REPRO-n19.md`). The `exh7` 18-item runs used the hardened `gt.c` (`n27/NOTES.md`).

**Consequence.** With Theorem 2, $f(10)\in[24,25]$: $f(10)\ge 24$ by [DSW] ($t(24,2,2)=10$), and $f(10)\le 25$ by $t(26,2,2)=11$ (a conditional candidate; its 17-item part rests on one implementation, exh7, and the 18-item part on the $n=27$ searches, where two implementations agree). Equivalently, $t(25,2,2)$ is 10 or 11 and is not determined here. (Before the $n=26$ run the bound was $f(10)\le 26$ via $t(27,2,2)=11$.)

## 6. Dependency chain

Each result is listed with exactly what it rests on. Input labels:

* **(hand)** hand proof, in this note or in the cited text;
* **(pub)** published result, cited, not re-proved here (unless "reproduced" is stated);
* **(neg)** computer: negative-search log (`designs=0`, `STATUS: COMPLETE`), with the implementations used; such logs are not independently checkable certificates, and agreement of implementations is evidence of consistency, not a step-by-step equivalence check (the programs share the enumeration/symmetry scheme);
* **(pos)** computer: positive witness, a concrete design checked by a standalone verifier (`search/verify.py`, `independent/verify.py`, brute force over all $2^n$ sets).

Implementations: X = CSP solver (`gt.c`, driven by `exh2/5/6/7.c`, and `cf.c` for the nonadaptive case); Y = colouring solver (`independent/gt2.c`, `gt2p.c`); A = the independent audit's own programs (`independent/auditor/`: `mysearch.cpp`, `mycff.cpp`), written from the same prune list.

**Results, bottom-up**

1. **Upper bounds, $t(n,2,2)\le 8,9,10,11$ for $n\le13,18,24,31$.** (pub) [DSW, Proposition 15]. (pos) our code reproduces the designs for $n=13,18,24$ (costs 8, 9, 10).
2. **$t(9,2,2)=7$, $t(10,2,2)=8$, $t(13,2,2)=8$, $t(15,2,2)=9$.** (pub) [DSW]. Reproduced: (pos) designs for $n=9$ (7 tests), 10 (8), 13 (8), 15 (9), 18 (9); (neg) X and Y find none with one test fewer. $f(7)=9$ rests on $t(9)=7$ and $t(10)=8$, taken from [DSW]; the audit did not re-derive $t(10,2,2)\ge8$ for small $k$.
3. **$f(1),\dots,f(6)=1,\dots,6$** ($f(0)=0$; $f(T)\ge T$ by individual testing). (neg) X and Y (own search code); re-derived by A with no $f$, Sperner or top-pool prune (`docs/AUDIT-n19.md`, 3d).
4. **Lemma A, Lemma B** (Section 4). (hand). Lemma A: principle in [DSW, Lemma 13] and [G] for one pool, the multi-pool/per-outcome form is ours; Lemma B follows from [DSW, Cor. 5] plus Sperner's theorem. Sanity checks only (not proofs): `search/lemma_check.py`, 0 violations on more than 16,000 designs; `n29` check on 18,139 designs.
5. **$t(14,2,2)=9$** (Proposition 1; also [G]). Lower bound $\ge9$: (pub) [G], hand proof. Reproduced here: $k=1$ (hand) via $t(8,2,1)=8$ [HH] and [DSW, Lemma 11]; $k=2,\dots,8$ (neg) X and Y with equal total node counts; A re-ran $k=5,\dots,8$ with the same totals (not $k=2,3,4$). Upper bound $t(14)\le t(15)=9$: (pub) [DSW].
6. **$f(8)=13$.** $t(13,2,2)=8$ (pub, [DSW]) and $t(14,2,2)=9$ (item 5).
7. **$g(9)=12$.** Upper bound ($\le12$): (pub) [DSW, Table 1, $N_1(9)=12$] and (neg) `cf 13 9` (X-family program `cf.c`, 647,436,550 nodes, no family of 13; also A's `mycff`, 29,943,420 nodes; re-run after hardening). Lower bound ($\ge12$): (pos) a Steiner triple system on 9 points, checkable by hand.
8. **$t(19,2,2)=10$** (Theorem 2; supported computer-assisted candidate, novelty provisional). Upper bound $\le10$: (pub) [DSW, Prop. 15, $m=7$]. Lower bound, no 9-test strategy, by case on $k$:
   * $k=1,\dots,4$: (hand) Lemma A with $f(8)=13$ (item 6) and Lemma B; arithmetic in Section 4. A Lemma-B-free cross-check (neg, X) completed only for $k=4$ (105,715 nodes).
   * $k=5$: (neg) X (`exh2`) and Y (`gt2p`), both 4,288,517 nodes, no pool-size assumption; not re-run by A.
   * $k=6,7,8$: (neg) X (`exh7`) and Y (`gt2p`), with pool windows from Lemma A, Lemma B and the counting prune $m_W\le f(q+|W|)$ (items 3, 6, 2 for $f(7)=9$); A's `mysearch` reports the same totals for $k=6,7,8$; for $k=6$ the $X$ and $Y$ totals differ (different prune sets) and are not compared.
   * $k=9$ (nonadaptive): item 7, $g(9)=12<19$. ($k=0$ coincides with $k=9$.)
9. **$t(20,2,2)=t(21,2,2)=10$** (Proposition 3; not new). (pub) [G], hand proof; also lower bounds from item 8 by monotonicity (restriction of a strategy) and upper bounds from item 1.
10. **$t(n,2,2)$ determined for all $n\le24$.** Items 1, 2, 5, 8, 9 together with [DSW] (conditional on item 8; to be re-verified against the papers).
11. **$t(29,2,2)=11$ and $t(28,2,2)=11$ (conditional candidates).** Upper bound: (pub) [DSW, Prop. 15, $m=8$]. Lower bound: (hand) Lemma A per-pool form with $f(9)=18$, whose upper half is Theorem 2 (item 8), and Lemma B, for $k=1..7$ and $k=8,9$ (counting; arithmetic re-derived by scripts `n28/derive_independent.py`, `n29/count_bound.py`); $k=10$ via item 7. No search beyond item 7.
12. **$t(27,2,2)=11$ (conditional candidate).** Upper bound: (pub) [DSW, Prop. 15]. Lower bound: (hand) as item 11 for $k\le7$ and $k=10$; (hand) forced stage-1 pool of exactly 9 items for $k=8,9$ (arithmetic rechecked by `n27/force9_check.py`); (hand) Lemma A reduction of a negative 9-pool to an 18-item 9-test strategy of shape $(k',q)=(8,1)$ or $(7,2)$ with pools of 5 to 10 items ($f(8)=13$, item 6); then (neg) exhaustive classification search of those two shapes: X (`exh7`) COMPLETE, 0 designs (`n27/logs6h/`); Y (`gt2p`, partitioned, no node limit) COMPLETE, 0 designs (60/60 and 150/150 parts, `n27/logs_gt2p/`). Also depends on Theorem 2 (item 8) and item 7.
13. **$t(26,2,2)=11$ (conditional candidate; one implementation for the 17-item part).** Upper bound: (pub) [DSW, Prop. 15]. Lower bound: (hand) $k=1..5$ and $k=10$ as items 11 and 12 (with $g(9)=12$, item 7); (hand) forced pool of exactly 8 for $k=6,7$ and a smallest pool of 8 or 9 for $k=8,9$ (arithmetic recomputed by script); (hand) Lemma A reduction of a negative pool to a 9-test strategy on 18 or 17 items with windows from $f(8)=13$ (item 6) and Lemma B; then (neg) classifications: $(18;5,4)$, $(18;6,3)$ (one design $D$) with X (`exh7`) and Y (`gt2p`), COMPLETE, and the extension of $D$ (X only) with 0 designs; the $n=27$ shapes $(18;7,2)$, $(18;8,1)$ with X and Y (item 12); $(17;7,2)$ and $(17;8,1)$ with X only, COMPLETE, 200/200 parts each, 0 designs (`n26/logs_full/`). Also depends on Theorem 2 (item 8) and item 7. Not peer reviewed; novelty provisional.
14. **$f(10)\in[24,25]$** (conditional). Lower end $t(24,2,2)=10$ (pub, [DSW]); upper end $t(26,2,2)=11$ (item 13). $n=25$ is open.

**Dependency graph (arrows point from an input to what uses it)**

    [DSW Prop 15] ------------------------> upper bounds for 14..31 (items 1,5,8,9,11,12)
    [DSW t(9)=7,t(10)=8,t(13)=8] ---> f(7)=9, f(8)=13 (with item 5) ---> Lemma A inputs
    [G] / our k=2..8 search ---> t(14)=9 ---> f(8)=13
    f(1..6) (search, audited) --+
    Lemma A, Lemma B (hand) ----+--> t(19)=10:  k=1..4 hand | k=5..8 search X,Y,(A) | k=9 g(9)=12
    [DSW Table 1] + cf search --> g(9)=12 ---> k=9 of n=19; k=10 of n=27,28,29
    t(19)=10 ---> f(9)=18 ---> t(29)=11, t(28)=11 (hand counting + g(9))
                          \--> t(27)=11 (hand counting + g(9) + 18-item search X, Y: both no design)
                          \--> t(26)=11 (hand counting + g(9) + 18-item searches X, Y + 17-item search X only + extension of D, X only)
    t(24)=10 [DSW] + t(26)=11 ---> f(10) in [24,25]

## 7. Caveats

* The proof is computer-assisted and has not been peer reviewed. Correctness rests on the code and on the reduction in (a)–(c); the two implementations share that reduction, the enumeration and symmetry scheme and the cap on candidate sets, and differ only in the second-stage feasibility solver. Agreement of total node counts between them is evidence of consistency, not a step-by-step equivalence check.
* For Theorem 2 the proof also depends on the lower-order values $f(1),\dots,f(6)$ (counting prune $m_W\le f(q+|W|)$), which come from our own search code and were re-derived by the audit's separately written program, but not by a second implementation of our own; and on Proposition 1 for $f(8)=13$. The cases $k=2,3$ and $k=1$ rest on Lemmas A and B (proved above); the cross-check without Lemma B was completed only for $k=4$. For $n=19$, total node counts agree between the two implementations for $k=5$, $k=7$ and $k=8$; for $k=6$ the runs use different prune sets (see above).
* Novelty of $t(19,2,2)=10$ is provisional. It is not found in [DSW] or [G], and no later work was found, but the search was limited; citation lists (Google Scholar, Semantic Scholar, OpenAlex "cited by") were not accessible. Check them before any claim of novelty. Lemma B is potentially new as a stated tool, checked only against [DSW] and [G].
* An independent audit (2026-10-18, `docs/AUDIT-n19.md`) gives the verdict SUPPORTED for the $n=19$ argument, with the gaps listed there ($f(7)=9$ is taken from [DSW]; $k=5$ was not re-run by the auditor).
* Hardening (2026-10-19, `docs/REPRO-n19.md`): a node-limit hit now returns UNKNOWN instead of infeasible, and an array boundary is fixed. The re-run slices give the same node counts and verdicts. The gt2p $k=6$ run and the `cf 13 9` run (the input $g(9)=12$ of Section 5) were re-run after the fixes and are COMPLETE with the same results.
* Section 5 is conditional on Theorem 2 and is not a separate result. Its $k=10$ case uses the computed value $g(9)=12$, a negative-search computation whose log is not an independently checkable certificate.
* The $n=27$ case of Section 5 additionally depends on a negative search (`exh7` and `gt2p`, two 18-item cases, 0 designs each). The second-solver cross-check (`gt2p`) has finished with 0 designs, so two independent implementations agree there is no design; these are negative-search logs, not independently checkable certificates, and $t(27,2,2)=11$ remains a conditional candidate, not peer reviewed, with provisional novelty.
* The $n=26$ case of Section 5 additionally depends on exh7 classifications of $(17;7,2)$ and $(17;8,1)$ (about 112 CPU-hours, 0 designs) for which no second implementation was run, and on an exh7-only extension step for $k=7$. These are negative-search logs, not independently checkable certificates. $t(26,2,2)=11$ is a conditional candidate (conditional on $t(19,2,2)=10$ and $g(9)=12$), not peer reviewed, with provisional novelty. The $n=25$ case is open.
* The search code was produced for this project (it is not a packaged tool); it hard-wires $d=2$ and two stages.

## Code availability

Source code and logs are available on request.

## Acknowledgements

*TODO (author): word per venue policy.* Computational assistance, including writing and running the search code and drafting this note, was provided by AI tools (Anthropic Claude models); the author is responsible for the content.

## References

* [DSW] P. Damaschke, A. Sheikh Muhammad, G. Wiener, Strict group testing and the set basis problem, J. Combin. Theory Ser. A 126 (2014) 70–91.
* [G] D. Grankvist, *Searching for Search Strategies: Solutions to Strict Group Testing Problem Instances*, MSc thesis, Chalmers / University of Gothenburg, May 2017 (supervisor P. Damaschke). Sections 5.1 (p.24) and 5.4 (pp.41-46).
* [HH] S. H. Huang, F. K. Hwang, When is individual testing optimal for nonadaptive group testing?, SIAM J. Discrete Math. 14 (2001) 540–548.
