# Evidence map

## Manuscript table
All entries below were checked directly against the included historical logs. Hashes establish identity, not search correctness.

| k | implementation | nodes | raw historical log |
|---|---|---:|---|
| 5 | exh2 | 4,288,517 | `repro_logs/n19/n19_k5_exh2.log` |
| 5 | gt2p | 4,288,517 | `repro_logs/n19/n19_k5_gt2p.log` |
| 6 | exh7 | 226,207 | `repro_logs/n19/n19_k6_exh7.log` |
| 6 | gt2p | 333,357 | `repro_logs/n19/n19_k6_gt2p.log` |
| 7 | exh7 | 13,504,468 | `repro_logs/n19/n19_k7_exh7.log` |
| 7 | gt2p | 13,504,468 | `repro_logs/n19/n19_k7_gt2p.log` |
| 8 | exh7 | 32,534,044 | `repro_logs/n19/n19_k8_exh7.log` |
| 8 | gt2p | 32,534,044 | `repro_logs/n19/n19_k8_gt2p.log` |

The current preparation freshly reran only the two k=6 rows: see `validation/fresh-n19-k6-exh7.log` and `validation/fresh-n19-k6-gt2p.log`. Both completed with zero designs and matched these node counts. No claim of a complete fresh n19 replay is made.

## Integrity and bounded checks
- `validation/historical-source-hashes.json`: all nine historical source hashes match current included sources. The historical manifest describes uncommitted changes; file hashes resolve this ambiguity for these nine source files, not for every environment input.
- `validation/regressions.log`: full bounded test suite passed; ASan/UBSan enabled, leak detection disabled.
- `validation/counting.log`: independent elementary weighted-deficit arithmetic.
- `repro_logs/n19/MANIFEST.txt`: 25 raw run logs, all matching recorded hashes. A complete folder is not a proof of complete mathematical case coverage.

## Other historical observations
The manuscript's reported unbounded-pool k=6 run (288,383,544 nodes) comes from the historical REPRO-n19 discussion, not a raw log bundled here. It is contextual only and is not needed for the theorem. The project also contains unsuccessful or incomplete exploratory runs outside this curated package.

## Proof dependencies
Negative-set and antichain lemmas: self-contained manuscript proofs. The n28/n29 conclusion is conditional on f(9)<=18; source-log verification does not discharge this hypothesis. Symmetry, pruning and reduction correctness still require mathematical validation.
