#!/bin/bash
# Validation of the n26 wide build (n26/gt.c MAXN 28, n26/exh7.c with F9/PREFIX/REQ/NOSING) against the existing search/ code and known results.
# Run: bash tests/test_n26.sh   (about 15 s). Installs nothing.
cd "$(dirname "$0")/.." || exit 1
T=$(mktemp -d "${TMPDIR:-/tmp}/n26-tests.XXXXXX"); trap 'rm -rf "$T"' EXIT
fails=0; ok(){ echo "PASS $1"; }; bad(){ echo "FAIL $1"; fails=$((fails+1)); }
chk(){ if eval "$2"; then ok "$1"; else bad "$1"; fi; }
gcc -O2 -DWIDE -o "$T/new" n26/exh7.c -lm || exit 1
gcc -O2 -DWIDE -o "$T/old" search/exh7.c -lm || exit 1
gcc -O2 -o "$T/gt26" n26/gt.c -lm || exit 1
done_line(){ grep '^DONE n=' "$1" | tail -1; }
D18="0 0 0 3 5 6 9 10 12 17 18 20 24 33 34 36 40 48"

echo "== A. identical node counts and verdicts, n26 build vs search/exh7.c (same flags)"
for sh in "18 5 4 5 6" "18 6 3 5 6" "17 5 4 4 7" "17 6 3 4 7" "18 4 5 5 5" "14 3 5 0 14" "14 4 4 0 14" "14 5 3 0 14"; do
  "$T/old" $sh > "$T/o.out" 2>&1; "$T/new" $sh > "$T/n.out" 2>&1
  chk "exh7 $sh: same DONE line (nodes, cspcalls, designs_found)" '[ -n "$(done_line $T/o.out)" ] && [ "$(done_line $T/o.out)" = "$(done_line $T/n.out)" ] && grep -q "^STATUS: COMPLETE" "$T/n.out"'
done
INTERCAP=2 "$T/new" 19 6 3 6 6 > "$T/n.out"
chk "n=19 k=6 (INTERCAP=2, REPRO-n19): nodes=226207 designs_found=0" 'grep -q "nodes=226207 .*designs_found=0" "$T/n.out"'
FIRST=1 "$T/new" 13 5 3 0 13 > "$T/n.out"
chk "f(8)>=13: n=13 k=5 q=3 design found (DONE(first) nodes=140)" 'grep -q "^FOUND sigs: 0 0 0 3 5 6 9 10 12 17 18 20 24$" "$T/n.out" && grep -q "DONE(first) nodes=140" "$T/n.out"'
"$T/new" 14 5 3 0 14 > "$T/n.out"
chk "f(8)<14 at k=5: n=14 k=5 q=3 designs_found=0, nodes=334880 (equals gt2p n=14 T=8 k=5)" 'grep -q "nodes=334880 .*designs_found=0" "$T/n.out"'

echo "== B. n=24 k=7 design (DSW 2014, Prop. 15) with the wide build, verified independently"
F9=18 FIRST=1 "$T/new" 24 7 3 6 9 > "$T/n24.out"
chk "n=24 k=7 q=3 pools 6..9 FIRST: design found (5,881 nodes)" 'grep -q "^FOUND sigs: 0 0 0 3 5 6 9 10 12 17 18 20 24 33 34 36 40 48 65 66 68 72 80 96$" "$T/n24.out" && grep -q "DONE(first) nodes=5881" "$T/n24.out"'
"$T/gt26" cert 7 3 "$T/c24.json" 0 0 0 3 5 6 9 10 12 17 18 20 24 33 34 36 40 48 65 66 68 72 80 96 > /dev/null 2>&1
chk "n=24 certificate from n26/gt.c passes search/verify.py with max_t=10 (VALID)" 'python3 -I search/verify.py "$T/c24.json" 10 | grep -q "^VALID"'

echo "== C. prefix extension (PREFIX, REQ=64, NOSING=1)"
PREFIX="$D18" REQ=64 NOSING=1 F9=18 "$T/new" 24 7 3 6 9 > "$T/e.out"
chk "positive control: D(18 items) + 6 new items in pool 7 -> finds exactly the n=24 design" 'grep -q "designs_found=1" "$T/e.out" && grep -q "^FOUND sigs: $D18 65 66 68 72 80 96$" "$T/e.out"'
PREFIX="0 0 0 3 5 6 9 10 12 17" NOSING=1 "$T/new" 18 6 3 5 6 > "$T/e.out"
chk "positive control: first 10 items of D fixed, n=18 k=6 q=3 -> completes to D, designs_found=1" 'grep -q "designs_found=1" "$T/e.out" && grep -q "^FOUND sigs: $D18\$" "$T/e.out"'
PREFIX="$D18" REQ=64 NOSING=1 F9=18 "$T/new" 25 7 3 7 9 > "$T/e.out"
chk "D + 7 items (n=25 k=7): designs_found=0, COMPLETE" 'grep -q "designs_found=0" "$T/e.out" && grep -q "^STATUS: COMPLETE" "$T/e.out"'
PREFIX="$D18" REQ=64 NOSING=1 F9=18 "$T/new" 26 7 3 8 9 > "$T/e.out"
chk "D + 8 items (n=26 k=7): designs_found=0, COMPLETE" 'grep -q "designs_found=0" "$T/e.out" && grep -q "^STATUS: COMPLETE" "$T/e.out"'
PREFIX="0 0 5 3" "$T/new" 18 6 3 5 6 > /dev/null 2>&1; rc=$?
chk "decreasing PREFIX rejected (exit 2)" '[ $rc -eq 2 ]'
PREFIX="$D18 64" "$T/new" 18 6 3 5 6 > /dev/null 2>&1; rc=$?
chk "PREFIX longer than n rejected (exit 2)" '[ $rc -eq 2 ]'

echo "== D. limits and status conventions of the wide build"
"$T/new" 29 7 3 0 29 > /dev/null 2>&1; rc=$?
chk "n=29 > MAXN=28 rejected (exit 2)" '[ $rc -eq 2 ]'
GT_NODELIMIT=3 "$T/new" 14 3 5 0 14 > "$T/n.out"; rc=$?
chk "tiny node limit: STATUS: INCOMPLETE, exit 3" 'grep -q "^STATUS: INCOMPLETE" "$T/n.out" && ! grep -q "^STATUS: COMPLETE" "$T/n.out" && [ $rc -eq 3 ]'
SAN=""; echo 'int main(){return 0;}' > "$T/s.c"; gcc -fsanitize=address,undefined "$T/s.c" -o "$T/s" 2>/dev/null && "$T/s" && SAN="-fsanitize=address,undefined"
gcc -g -O1 $SAN -DGT_SRC="\"../n26/gt.c\"" -o "$T/tb" tests/test_boundary.c -lm 2>/dev/null; "$T/tb" > "$T/tb.out" 2>&1; rc=$?
chk "n26/gt.c: n=MAXN=28 boundary test clean${SAN:+ (sanitizers on)}" '[ $rc -eq 0 ] && grep -q "BOUNDARY OK" "$T/tb.out" && grep -q "^n=28 " "$T/tb.out"'
gcc -g -O1 $SAN -DWIDE -o "$T/newsan" n26/exh7.c -lm 2>/dev/null
PREFIX="$D18" REQ=64 NOSING=1 F9=18 "$T/newsan" 26 7 3 8 9 > "$T/san.out" 2>&1; rc=$?
chk "n=26 extension run clean under sanitizers (n=26 uses indices up to 26 of arrays sized MAXN+1)" '[ $rc -eq 0 ] && ! grep -q "ERROR\|runtime error" "$T/san.out"'

echo "== summary: $fails failure(s)"
exit $((fails>0))
