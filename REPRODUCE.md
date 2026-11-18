# Reproduction guide

Run from the extracted folder. Requires Python 3, Bash, GCC and G++; full slice reruns additionally require GNU timeout and sha256sum. No dependencies are installed automatically. The PDF is supplied; compiling the TeX requires a standard LaTeX distribution with its declared packages.

## Bounded validation
```
sha256sum -c SHA256SUMS
python3 -I tests/test_verify.py
python3 -I tests/check_manifest.py
ASAN_OPTIONS=detect_leaks=0 bash tests/run_tests.sh
python3 -I validation/check_counting.py
```
The aggregate test compiles into temporary directories, tests node-limit/UNKNOWN behavior, array boundaries, witness budgets, stored-log hashes, partition controls and small wide-build controls. It can take several minutes. It does not rerun every n19 lower-bound slice. An old-code contrast is skipped in this history-free package; no git history is required for current-code tests.

## Historical n19 evidence and optional costly replay
`repro_logs/n19/` contains the complete upstream n19 reproduction folder, including incomplete runs. The manifest is verified byte-for-byte against included files. Its completeness as a folder does not prove mathematical coverage.
```
bash reproduce_n19.sh list
```
This lists slices and estimates. For an intentional rerun, work in a disposable copy: `bash reproduce_n19.sh quick`, `medium`, `long` or an individual named slice. These commands write/overwrite `repro_logs/n19/` and can take minutes to hours. Defaults include timeouts; inspect return status and log completion for every slice. Generating a new manifest cannot turn incomplete runs into proof.

Some historical helper scripts and documentation refer to the full research repository. Only the bounded commands above and the n19 replay entry point are supported by this curated release; n26/n27 exhaustive log distributions are intentionally absent. Source for n26 is retained because regression tests use its wide-build controls, not to claim that n26 has been proved.

The validation environment runs under tracing, which prevents LeakSanitizer initialization. The recorded final run sets ASAN_OPTIONS=detect_leaks=0; this retains AddressSanitizer and UndefinedBehaviorSanitizer while excluding leak detection. The initial unsanitized fallback log is retained separately.

## Fresh narrow replay
`bash validation/replay_k6.sh` compiles both implementations in a disposable temporary directory and reruns only n19, k=6, q=3. It does not overwrite historical logs. The included fresh output records their zero-design outcomes. The full n19 lower bound is not validated by this narrow replay.
