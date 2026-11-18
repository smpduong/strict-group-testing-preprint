#!/bin/bash
set -eu
cd "$(dirname "$0")/.."
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
gcc -O2 independent/gt2p.c -o "$T/gt2p"
gcc -O2 search/exh7.c -lm -o "$T/exh7"
"$T/gt2p" search 19 9 6 0 6 6 1 > "$T/gt2p.log"
INTERCAP=2 "$T/exh7" 19 6 3 6 6 > "$T/exh7.log"
grep -q 'nodes=333357 ' "$T/gt2p.log"
grep -q 'designs=0' "$T/gt2p.log"
grep -q '^STATUS: COMPLETE' "$T/gt2p.log"
grep -q 'nodes=226207 ' "$T/exh7.log"
grep -q 'designs_found=0' "$T/exh7.log"
grep -q '^STATUS: COMPLETE' "$T/exh7.log"
cat "$T/gt2p.log" "$T/exh7.log"
echo 'PASS: two k=6 slices only; not a complete n19 validation'
