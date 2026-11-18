#!/bin/bash
# Reproduces the exhaustive negative result for n=14, total 8 tests, every split k (stage-1 pools) + q=8-k (stage-2 pools).
cd "$(dirname "$0")"
gcc -O2 -o gt gt.c -lm && gcc -O2 -o exh2 exh2.c -lm && gcc -O2 -DWIDE -o exh2w exh2.c -lm
python3 sanity.py            # known-case reproduction (Prop 15 designs, K4+3, C4, K_{2,k})
python3 bf_check.py          # CSP r(W) vs independent brute-force set cover
for k in 1 2 3 4 5 6 7 8; do q=$((8-k)); echo -n "k=$k q=$q: "; ./exh2w 14 $k $q 0 14 | tail -2 | tr '\n' ' '; done
