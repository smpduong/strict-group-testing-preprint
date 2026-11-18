> Historical source document, retained for context. Current release status is in ../README.md. AI review only; no independent human review. Links may refer to the full private research repository, not this curated subset.

# Reproducing n=19 (t(19,2,2)=10)

## From a fresh checkout

- `bash tests/run_tests.sh` checks the node-limit fix, the array-boundary fix and the verifier. It runs with ASan/UBSan where gcc supports them.
- `bash reproduce_n19.sh quick` takes about 25 min on 4 cores.
- `bash reproduce_n19.sh medium` takes about 30 min. It covers n19_k5_exh2, n19_k8_exh7, n14_k6/7/8 and aud_n19_k8.
- `bash reproduce_n19.sh list|long|manifest|stored|<slice>` gives the other options. `stored` hashes the committed logs into `repro_logs/n19/STORED_LOGS.txt`.

## Recorded per run

Every run is recorded in `repro_logs/n19/MANIFEST.txt`: All raw logs (`*.log`, `*.err`) are committed (not git-ignored); `python3 tests/check_manifest.py` verifies every entry's sha256.
- commit
- compiler: gcc 13.3.0 (Ubuntu 24.04)
- flags: `-O2` (`-DWIDE` for exh2w). Auditor files: `g++ -O2 -std=c++17`.
- node limit: 2,000,000,000 per r(W) check, in `search/gt.c` and `n29/gt.c` only. `gt2*.c` and `cf.c` have no limit.
- sha256 of each source file: `search/gt.c exh2.c exh7.c cf.c`, `independent/gt2.c gt2p.c`, auditor `core.hpp mysearch.cpp mycff.cpp`
- for each run: the exact command, the completion flag (`STATUS: COMPLETE` or `STATUS: INCOMPLETE (node limit)`, exit code 3), the log's sha256 and the node counts

## Commands

Run these from `repro_build/`:

```
exh2 19 5 4 0 19
INTERCAP=2 exh7 19 6 3 6 6
exh7 19 7 2 6 9
exh7 19 8 1 6 9
gt2p search 19 9 {5 0 0 19 1 | 6 0 6 6 1 | 7 0 6 9 1 | 8 0 6 9 1}
cf 13 9
n=14: exh2(w) 14 k 8-k 0 14
```

## Status of each slice

| Slice | Evidence | Status |
|---|---|---|
| k=1..4 | Analytic (Lemmas A and B) | No run needed |
| k=5 | exh2 and gt2p, both 4,288,517 nodes | COMPLETE |
| k=6 | exh7 (INTERCAP=2): 226,207 nodes; auditor mysearch agrees. gt2p with pools 6..6 (forced by Lemma B): 333,357 nodes, re-run after the fixes (3.5 s). gt2p with no pool bounds: 288,383,544 nodes (stored log `independent/logs/n19full/k6.log`, 5,362 s) | COMPLETE |
| k=7 | exh7, gt2p and auditor, all 13,504,468 nodes | COMPLETE |
| k=8 | exh7, gt2p and auditor, all 32,534,044 nodes | COMPLETE |
| k=9 | Nonadaptive: needs g(9)=12. `cf 13 9` re-run after the fixes: 647,436,550 nodes, no family, 781 s. Auditor `mycff 9 13` agrees. | COMPLETE |
| Auditor's own k=5 | Killed at about 28 min, empty log | INCOMPLETE. Not evidence. |

Other notes:
- `independent/logs/n19full/k7.log` and `k8.log` (no pool bounds) timed out (exit 124). They are incomplete and are not evidence.
- `search/logs/exh2_14_2_6_p0-14.log` is empty.
- `independent/run_n19.sh` reads `n19jobs.txt`, which was never kept.
- Logs older than 2026-10-19 predate the STATUS line.

## Two kinds of artefact

- **Positive-strategy witnesses:** `search/cert_*.json` and `n29/cert16_1.json`. `verify.py` checks them independently.
- **Negative-search logs:** `designs_found=0` / `designs=0`. These are not independently checkable impossibility certificates. Agreement between implementations is evidence, not a certificate.

## The auditor's sources

They are in `independent/auditor/`, recovered from the session scratchpad. See `NOTE.txt` there. The auditor's original compiler flags were not recorded.

## Fixes made 2026-10-19 (reviewer findings)

1. **Node limit.** Hitting the node limit used to return "infeasible". It now returns UNKNOWN. UNKNOWN never prunes, and the run ends INCOMPLETE with exit code 3. Tested with `GT_NODELIMIT=3` on a case where a design exists: the old code printed `designs_found=0`, and the new code prints INCOMPLETE.
2. **Array boundary.** `static int code[MAXN]` was written at index MAXN. It is now `code[MAXN+1]`, with an ASan boundary test in `tests/test_boundary.c`. The bug needs m = MAXN, that is n >= 24 in `search/gt.c`. No n=14 or n=19 run reached it.
3. **Results unchanged.** Every re-run slice above gives the same node counts and verdicts as before the fixes.
