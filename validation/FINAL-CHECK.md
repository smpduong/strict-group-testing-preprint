> **Authorization update:** Earlier licence/publication gates below describe preparation history. The author has now explicitly authorized MIT code and CC BY 4.0 preprint licensing and publication as a fresh curated repository. See LICENSING.md and PUBLIC-RELEASE.md. Scientific evidence caveats remain unchanged.

# Final preprint preparation check, 18 November 2026

The manuscript was compiled twice and all five rendered pages visually inspected. No clipping or overlaps were observed. References resolve. Two bibliography underfull-line notices are harmless spacing notices.

The full bounded regression suite passed with zero failures and ASan/UBSan enabled, leak detection disabled for this traced environment. See final-regressions.log. A separate AI-assisted review reran the two k=6 slices, reproduced 226207 and 333357 nodes with completion and zero designs, and found no gap in the stated conditional counting argument. See final-k6-replay.log. These checks are neither independent human review nor certification of the full n19 enumeration. No full n19 replay was performed.

Final edits removed stale no-upload wording and clarified embedded TeX font resources. The latest revision removes an unnecessary nonadaptive-capacity hypothesis using the existing antichain lemma. Solver algorithms are unchanged. The n19 result remains a candidate and the n28/n29 result explicitly conditional on f(9)<=18. Focused literature checking is not exhaustive novelty clearance.

The archive excludes Git history, credentials, correspondence and local build intermediates. Author identity and public repository name are intentional. Automated pattern checks cannot guarantee privacy or rights clearance. No standalone third-party papers or solver binaries are bundled. Author-created source licensing still requires an author decision before public release.
