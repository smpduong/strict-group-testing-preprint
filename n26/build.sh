#!/bin/bash
# Builds the n26 wide binaries into n26/bin/ (gcc -O2; -DWIDE = 128-bit domains, q<=7). Same flags as the existing exh2w build.
cd "$(dirname "$0")" || exit 1
mkdir -p bin
gcc -O2 -DWIDE -o bin/exh7w26 exh7.c -lm || exit 1
gcc -O2 -DWIDE -o bin/gt26 gt.c -lm || exit 1
gcc -O2 -o ../n26/bin/gt2p ../independent/gt2p.c || exit 1
echo "built: bin/exh7w26 bin/gt26 bin/gt2p"
sha256sum exh7.c gt.c ../independent/gt2p.c
