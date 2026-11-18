# Strict two-stage group testing: methods and conditional results

Curated preprint software snapshot, 2026-11-18. See STATUS.md for evidentiary limits.

This curated subset accompanies **Counting Reductions and Computational Evidence for Small Two-Stage Strict Group-Testing Instances** (`manuscript.pdf`, editable `manuscript.tex`). It is not a certified set of new exact values. AI-produced and AI-reviewed; no independent human review or institutional endorsement is claimed.

## Start here
- `STATUS.md`: precise claim/evidence boundaries.
- `REPRODUCE.md`: commands that work in this curated subset and what they establish.
- `RELEASE-AUDIT.md`: provenance, exclusions, privacy and licensing limitations.
- `validation/`: fresh check results. `repro_logs/n19/`: historical upstream logs, not fresh reruns.
- `SHA256SUMS`: integrity only, not mathematical validity.

The n28/n29 result is conditional: **if f(9) <= 18, then t(28,2,2) = t(29,2,2) = 11**. The manuscript defines f and t. The n19 lower bound remains a computational candidate whose exhaustive enumeration and reductions need further validation.

## Release boundary
Do not flip the original working repository to public. This history-free subset excludes commit messages, session links, private correspondence, author outreach and broad unfinished n25/n26/n27 logs. The original repository remains unchanged. Any deposit metadata should identify this exact archive, not imply that the original working repository is public.

Code and associated software documentation: MIT. Preprint PDF/TeX: CC BY 4.0. See LICENSE, LICENSE-MANUSCRIPT.md and LICENSING.md.

## Presentation-quality revision
The five-page manuscript now reports the current evidence package rather than the earlier incomplete reading snapshot. It includes explicit candidate/conditional wording, improved prior-work attribution, and no internal instruction to the author. The two k=6 implementations were freshly rerun and reproduced their recorded node totals and zero-design outcomes. Nine historical source hashes and eight table entries were checked directly. See EVIDENCE-MAP.md. No algorithm or mathematical theorem was changed, and no public release was performed.

## Reproduction and history
Use make check for integrity and quick validation; make test runs the bounded sanitizer regression suite. No dependency installation or network access is performed by these targets.

This snapshot starts fresh history. The original private research repository is not published or rewritten. See PUBLIC-RELEASE.md for the release scope and historical audit limits.
