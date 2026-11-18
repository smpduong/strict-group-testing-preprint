#!/bin/bash
# consistency of exhaustive search with known t values: t(13,2,2)=8 (so 7 tests impossible), t(9,2,2)=7, t(10,2,2)=8
cd "$(dirname "$0")"
for tot in 7; do for k in 1 2 3 4 5 6 7; do q=$((tot-k)); echo -n "n=13 total=$tot k=$k q=$q : "; ./exh2w 13 $k $q 0 13 | tail -2 | tr '\n' ' '; done; done
for k in 1 2 3 4 5 6; do q=$((6-k)); echo -n "n=9 total=6 k=$k q=$q : "; ./exh2w 9 $k $q 0 9 | tail -2 | tr '\n' ' '; done
for k in 1 2 3 4 5 6 7; do q=$((7-k)); echo -n "n=10 total=7 k=$k q=$q : "; ./exh2w 10 $k $q 0 10 | tail -2 | tr '\n' ' '; done
echo -n "n=9 total=7 k=4 q=3 (expect found): "; FIRST=1 ./exh2w 9 4 3 0 9 | tail -2 | tr '\n' ' '
echo -n "n=18 total=9 k=6 q=3 (expect found): "; FIRST=1 ./exh2w 18 6 3 0 18 | tail -2 | tr '\n' ' '
