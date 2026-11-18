> **Authorization update:** Earlier licence/publication gates below describe preparation history. The author has now explicitly authorized MIT code and CC BY 4.0 preprint licensing and publication as a fresh curated repository. See LICENSING.md and PUBLIC-RELEASE.md. Scientific evidence caveats remain unchanged.

# Release preparation audit

Prepared 2026-11-18 from smpduong/strict-group-testing commit 9ac7b4bfc7530719ff66e4cabe5f0ff1ddfdaec2. Read-only upstream access; no push, visibility change or original-file edits during curation.

## Exact scope
114 selected upstream files were fetched at the pinned commit. Their content was verified against Git blob SHA-1 before curation. `UPSTREAM-FILES.json` records their paths, sizes and original object hashes. The full tree has 1,200 files; this is explicitly a curated subset, not a full clone. It includes the complete stored n19 reproduction folder, regression dependencies, selected code, positive witnesses and counting arguments. Current manuscript/source and release documentation were added.

## Changes
- Added a cautious README, current claim ledger, reproduction guide and licensing gate.
- Labelled historical documents so old claims are not mistaken for current release conclusions.
- Fixed n28/REPORT.md: 8·2·1 + 28·2·2 + 30·2·3 = 308. The original omitted the second factor 2 in the final term. This matches the manuscript and does not change the conditional conclusion.
- Added a direct counting-arithmetic regression.
- No solver algorithm changes; historical numerical logs remain unchanged.

## Privacy and integrity
No .git directory or commit messages are included. This avoids exporting historical Claude session URLs identified in the working repository's commit messages. No faculty correspondence, contact lists, access credentials, personal filesystem paths or source conversation transcripts are intentionally included. A pattern scan is recorded under validation; automated scanning is not a guarantee of absence. Review PDF metadata and author identity before publication. Upstream content beyond the selected files and full Git history has not undergone a comprehensive privacy audit.

## Remaining release gates
1. Author reviews mathematics, AI disclosure, author information and license choices.
2. Run and record complete n19 evidence replay if promoting the candidate beyond the current qualified scope; validate the reductions independently.
3. Keep n28/n29 premises in all public metadata; do not advertise negative logs as proof certificates.
4. Recheck current literature before asserting novelty. No exhaustive new novelty search was conducted during cleanup.
5. Recompute checksums after changes; verify final deposit's exact file list and metadata. Verify any subsequently prepared draft separately.

See validation/regressions.log and validation/counting.log for observed current checks. These checks do not constitute independent human peer review.

Fresh validation: the initial aggregate suite passed without sanitizers after LeakSanitizer rejected the traced runtime. A second run disables only leak detection to enable ASan/UBSan; see its final summary. Historical manifest checks cover 25/25 entries. The counting regression checks 308 > 248 and 349 > 279. PDF metadata contains no personal path, email, JavaScript or attachment; its displayed author is intentionally part of the manuscript.

Final sanitized aggregate run: exit 0, summary 0 failures, ASan/UBSan enabled with leak detection disabled. Only the two k=6 implementations were freshly replayed in the presentation-quality revision; no complete n19 replay was run.

## Presentation-quality revision
The five-page manuscript now reports the current evidence package rather than the earlier incomplete reading snapshot. It includes explicit candidate/conditional wording, improved prior-work attribution, and no internal instruction to the author. The two k=6 implementations were freshly rerun and reproduced their recorded node totals and zero-design outcomes. Nine historical source hashes and eight table entries were checked directly. See EVIDENCE-MAP.md. No algorithm or mathematical theorem was changed, with publication handled separately from artifact preparation.

## Final deposit preparation
See validation/FINAL-CHECK.md for the final repeated regression and k=6 review. Stale manuscript upload-status wording was removed without mathematical changes.

## Author-requested antichain simplification
Removed the redundant g(9)<=12 hypothesis from the abstract, theorem and discussion. The nonadaptive endpoints now follow directly from Lemma 2: 171>126 and 378>252. Historical notes retain their old reasoning with explicit supersession notices. The sole theorem premise f(9)<=18 and all candidate/certification caveats remain. The arithmetic checker now checks both endpoints. No solver algorithm or historical numerical log changed.
