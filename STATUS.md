# Claim and evidence ledger

| Claim | Release status | Evidence and remaining dependency |
|---|---|---|
| Negative-set restriction and antichain bounds | Proved in manuscript, AI checked | Self-contained proofs; author review needed |
| t(14,2,2)=9; t(20,2,2)=t(21,2,2)=10 | Prior published/thesis results, not new claims | Damaschke et al. and Grankvist; numerical checks are reproductions |
| t(19,2,2)=10 | Supported computational candidate | Historical negative-search logs; enumeration, symmetry and pruning are not independently certified here |
| If f(9)<=18, then t(28,2,2)=t(29,2,2)=11 | Conditional theorem in manuscript | The premise is explicit; do not remove it from metadata/abstract |
| n26/n27 exact-value claims | Not promoted by this release | Conditional reductions and additional negative searches outside current manuscript scope |
| n25 | Open in the source project's current status | Progress logs do not resolve it |

Two separately written AI implementations are not independent human validation and share mathematical enumeration assumptions. A positive witness verifier establishes that witness's upper bound, not an exhaustive lower bound. A timeout, node limit or incomplete partition yields UNKNOWN, never impossibility.

The fresh regression suite checks software safeguards and small controls. Historical logs retain their original status and are not relabelled as new runs. `docs/` contains historical discussions; this ledger and the accompanying manuscript take precedence for this release.
