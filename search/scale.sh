#!/bin/bash
cd "$(dirname "$0")"
k=$1; q=$2
for n in $(seq $3 $4); do
  s=$(date +%s)
  r=$(FIRST=1 ./exh $n $k $q 0 $n | tail -2 | tr '\n' ' ')
  e=$(date +%s)
  echo "k=$k q=$q n=$n : $r  ($((e-s))s)" >> logs/scale_k${k}_q${q}.log
done
