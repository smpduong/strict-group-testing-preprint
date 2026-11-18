#!/bin/bash
# Regression tests for: (1) node-limit => UNKNOWN/INCOMPLETE, (2) off-by-one at MAXN, (3) verifier fixes.
# Run from anywhere:  bash tests/run_tests.sh      (needs gcc, python3; ASan/UBSan if gcc supports them; installs nothing)
cd "$(dirname "$0")/.." || exit 1
T=$(mktemp -d "${TMPDIR:-/tmp}/sgt-tests.XXXXXX"); trap 'rm -rf "$T"' EXIT
fails=0
ok()   { echo "PASS $1"; }
bad()  { echo "FAIL $1"; fails=$((fails+1)); }
chk()  { if eval "$2"; then ok "$1"; else bad "$1"; fi; }

SAN=""; echo 'int main(){return 0;}' > "$T/s.c"
gcc -fsanitize=address,undefined "$T/s.c" -o "$T/s" 2>/dev/null && "$T/s" && SAN="-fsanitize=address,undefined" && echo "sanitizers: ON ($SAN)" || echo "sanitizers: not available, boundary tests run without them"

echo "== 1. node limit => UNKNOWN / INCOMPLETE"
gcc -O2 -o "$T/exh2" search/exh2.c -lm || exit 1
gcc -O2 -o "$T/exh7" search/exh7.c -lm || exit 1
gcc -O2 -o "$T/n29exh7" n29/exh7.c -lm || exit 1
gcc -O2 -o "$T/gt" search/gt.c -lm || exit 1
# negative instance: n=14, k=3, q=5 (no design; 1406 nodes with the default limit)
"$T/exh2" 14 3 5 0 14 > "$T/def.out"; rc=$?
chk "default limit: STATUS: COMPLETE, exit 0, nodes=1406, designs_found=0" 'grep -q "^STATUS: COMPLETE" "$T/def.out" && [ $rc -eq 0 ] && grep -q "nodes=1406 .*designs_found=0" "$T/def.out"'
GT_NODELIMIT=3 "$T/exh2" 14 3 5 0 14 > "$T/tiny.out"; rc=$?
chk "tiny limit (3): STATUS: INCOMPLETE, exit 3, no 'STATUS: COMPLETE'" 'grep -q "^STATUS: INCOMPLETE" "$T/tiny.out" && ! grep -q "^STATUS: COMPLETE" "$T/tiny.out" && [ $rc -eq 3 ]'
# positive instance n=9,k=4,q=3 (a design exists): the OLD code with a tiny limit claims "designs_found=0" (false impossibility)
git show HEAD:search/gt.c > "$T/gt.c" 2>/dev/null && git show HEAD:search/exh2.c > "$T/exh2.c" 2>/dev/null
if [ -s "$T/gt.c" ] && grep -q 'nodelimit=2000000000L' "$T/gt.c"; then
  sed -i 's/nodelimit=2000000000L/nodelimit=3L/' "$T/gt.c"; gcc -O2 -o "$T/exh2_old" "$T/exh2.c" -lm
  "$T/exh2_old" 9 4 3 0 9 | tail -1 > "$T/old.out"
  chk "OLD code, limit forced to 3, n=9 k=4 q=3 (design exists): reports designs_found=0 with no warning (the hazard)" 'grep -q "designs_found=0" "$T/old.out"'
else echo "(old-code contrast skipped: HEAD already contains the fix)"; fi
GT_NODELIMIT=3 "$T/exh2" 9 4 3 0 9 > "$T/new9.out"; rc=$?
chk "NEW code, limit 3, same instance: STATUS: INCOMPLETE, exit 3" 'grep -q "^STATUS: INCOMPLETE" "$T/new9.out" && [ $rc -eq 3 ]'
GT_NODELIMIT=3 "$T/exh7" 14 3 5 0 14 > "$T/e7.out"; rc=$?
chk "exh7 tiny limit: INCOMPLETE, exit 3" 'grep -q "^STATUS: INCOMPLETE" "$T/e7.out" && [ $rc -eq 3 ]'
GT_NODELIMIT=3 "$T/n29exh7" 14 3 5 0 14 > "$T/n29.out"; rc=$?
chk "n29/exh7 tiny limit: INCOMPLETE, exit 3" 'grep -q "^STATUS: INCOMPLETE" "$T/n29.out" && [ $rc -eq 3 ]'
GT_NODELIMIT=1 "$T/gt" eval 2 0 1 1 2 2 2 2 2 > "$T/ev.out"; rc=$?
chk "gt eval with limit 1: r=UNKNOWN printed, STATUS: INCOMPLETE" 'grep -q "r=UNKNOWN" "$T/ev.out" && grep -q "^STATUS: INCOMPLETE" "$T/ev.out" && [ $rc -eq 3 ]'
"$T/gt" eval 2 0 1 1 2 2 2 2 2 > "$T/ev2.out"; rc=$?
chk "gt eval default limit: no UNKNOWN, STATUS: COMPLETE" '! grep -q UNKNOWN "$T/ev2.out" && grep -q "^STATUS: COMPLETE" "$T/ev2.out" && [ $rc -eq 0 ]'

echo "== 2. off-by-one at the maximum supported size (n = MAXN)"
for src in search/gt.c n29/gt.c; do
  gcc -g -O1 $SAN -DGT_SRC="\"../$src\"" -o "$T/tb" tests/test_boundary.c -lm 2>"$T/cc.err" || { bad "compile boundary test for $src"; continue; }
  "$T/tb" > "$T/tb.out" 2>&1; rc=$?
  chk "$src: n=MAXN boundary test clean (rc=0, no sanitizer report)" '[ $rc -eq 0 ] && grep -q "BOUNDARY OK" "$T/tb.out" && ! grep -q "ERROR\|runtime error" "$T/tb.out"'
done
if [ -n "$SAN" ] && [ -s "$T/gt.c" ]; then
  git show HEAD:search/gt.c > "$T/gt_old.c"
  if grep -q 'static int code\[MAXN\];' "$T/gt_old.c"; then
    gcc -g -O1 $SAN -DGT_SRC="\"$T/gt_old.c\"" -o "$T/tb_old" tests/test_boundary.c -lm 2>/dev/null
    "$T/tb_old" > "$T/tbo.out" 2>&1
    chk "OLD gt.c at n=MAXN: sanitizer reports the out-of-bounds write (confirms the bug)" 'grep -q "global-buffer-overflow\|out of bounds" "$T/tbo.out"'
  fi
fi
"$T/exh2" 25 3 5 0 25 > /dev/null 2>"$T/e.err"; rc=$?
chk "n=MAXN+1 (25) is rejected with exit 2 instead of overflowing" '[ $rc -eq 2 ] && grep -q "outside supported range" "$T/e.err"'
# independent solvers at their maximum n under sanitizers
for spec in "independent/gt2.c 16 gt2" "independent/gt2p.c 20 gt2p"; do
  set -- $spec; gcc -O1 -g $SAN -o "$T/$3" "$1" || { bad "compile $1"; continue; }
  if [ "$3" = gt2 ]; then "$T/$3" search $2 7 4 > "$T/i.out" 2>&1; else "$T/$3" search $2 7 4 0 0 $2 1 > "$T/i.out" 2>&1; fi; rc=$?
  chk "$1 at n=$2 (its MAXN) under sanitizers: clean and STATUS printed" '[ $rc -eq 0 ] && grep -q "^STATUS:" "$T/i.out" && ! grep -q "ERROR\|runtime error" "$T/i.out"'
done

echo "== 4. certificate generator aborts when the stage-2 budget is too small"
# n=13 known design (k=5, 10 pairs + 3 empty sigs) needs q=3 stage-2 tests; budget 2 must fail with no certificate
SIG13="3 5 6 9 10 12 17 18 20 24 0 0 0"
for src in search/gt.c n29/gt.c; do
  gcc -O2 -o "$T/gtc" "$src" -lm || { bad "compile $src"; continue; }
  rm -f "$T/c_ok.json" "$T/c_small.json" "$T/c_small.json.tmp"
  "$T/gtc" cert 5 3 "$T/c_ok.json" $SIG13 > /dev/null 2>&1; rc=$?
  chk "$src cert with sufficient budget (q=3): exit 0, certificate verifies" '[ $rc -eq 0 ] && python3 -I search/verify.py "$T/c_ok.json" 8 >/dev/null'
  "$T/gtc" cert 5 2 "$T/c_small.json" $SIG13 > /dev/null 2> "$T/c.err"; rc=$?
  chk "$src cert with too-small budget (q=2): nonzero exit, no certificate file" '[ $rc -ne 0 ] && [ ! -e "$T/c_small.json" ] && [ ! -e "$T/c_small.json.tmp" ] && grep -q "budget" "$T/c.err"'
  echo stale > "$T/c_small.json"; "$T/gtc" cert 5 2 "$T/c_small.json" $SIG13 > /dev/null 2>&1
  chk "$src failed cert run deletes a stale certificate at the same path" '[ ! -e "$T/c_small.json" ]'
  GT_NODELIMIT=1 "$T/gtc" cert 5 3 "$T/c_unk.json" $SIG13 > /dev/null 2>&1; rc=$?
  chk "$src cert with solver UNKNOWN (node limit 1): nonzero exit, no certificate" '[ $rc -ne 0 ] && [ ! -e "$T/c_unk.json" ]'
done

echo "== 3. verifiers"
python3 tests/test_verify.py > "$T/v.out" 2>&1; rc=$?; tail -n 3 "$T/v.out"
chk "verifier tests (max_t enforcement, labels, sizes<=3 empirical)" '[ $rc -eq 0 ]'

echo "== 5. repro manifest"
chk "every repro_logs/n19/MANIFEST.txt entry has a raw log with matching sha256" 'python3 -I tests/check_manifest.py'

echo "== 6. gt2p work partitioning"
bash tests/test_gt2p_partition.sh > "$T/gp.out" 2>&1; rc=$?; tail -n 2 "$T/gp.out"
chk "gt2p partition tests (sums over parts == unpartitioned run, positive controls)" '[ $rc -eq 0 ]'

echo "== 7. n26 wide build (MAXN 28, F9/PREFIX/REQ/NOSING) vs search/exh7.c and known results"
bash tests/test_n26.sh > "$T/n26.out" 2>&1; rc=$?; grep -c '^PASS' "$T/n26.out" | sed 's/^/n26 PASS lines: /'; grep '^FAIL' "$T/n26.out"
chk "n26 validation tests (tests/test_n26.sh)" '[ $rc -eq 0 ]'

echo "== summary: $fails failure(s)"
exit $((fails>0))
