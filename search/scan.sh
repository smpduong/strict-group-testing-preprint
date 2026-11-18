#!/bin/bash
# scan.sh k q nstart nend : exhaustive (no pool bounds) for increasing n, logs result per n
cd "$(dirname "$0")"
for n in $(seq $3 $4); do
  s=$(date +%s)
  r=$(FIRST=1 ./exh5 $n $1 $2 0 $n | tail -2 | tr '\n' ' ' | cut -c1-260)
  echo "n=$n k=$1 q=$2 : $r  ($(( $(date +%s)-s ))s)" >> logs19/scan_k$1_q$2.log
  case "$r" in *"designs_found=0"*) break;; esac
done
