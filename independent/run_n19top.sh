#!/bin/bash
cd "$(dirname "$0")"   # independent/ ; binaries and logs are relative to it
[ -x ./gt2p ] || gcc -O2 -o gt2p gt2p.c || exit 1
s=$(date +%s)
printf "7 6 9\n8 6 9\n" | xargs -P 2 -L 1 bash -c 'timeout 10800 ./gt2p search 19 9 $0 0 $1 $2 1 > logs/n19top/k$0.log 2>&1; echo "exit=$?" >> logs/n19top/k$0.log'
echo "ALLDONE wall=$(( $(date +%s) - s ))s" > logs/n19top/ALLDONE
