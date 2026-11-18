#!/bin/bash
cd "$(dirname "$0")"
[ -x ./gt2 ] || gcc -O2 -o gt2 gt2.c || exit 1
# exists(n,T): any k in 1..T with a design (first=1). none(n,T): full enumeration all k.
exists() { for k in $(seq 1 $1 ); do :; done; }
for T in 1 2 3 4 5 6; do
  for n in $(seq 1 $((T+1))); do
    ex=0; for k in $(seq 1 $T); do r=$(./gt2 search $n $T $k 1 | grep -c DESIGN); if [ "$r" -gt 0 ]; then ex=1; break; fi; done
    echo "T=$T n=$n exists=$ex"
  done
done
