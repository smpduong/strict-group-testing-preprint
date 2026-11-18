#!/bin/bash
cd "$(dirname "$0")"   # independent/ ; binaries and logs are relative to it
[ -x ./gt2p ] || gcc -O2 -o gt2p gt2p.c || exit 1
mkdir -p logs/n19
# n19jobs.txt (one "k minp maxp" per line) was not kept; see ../reproduce_n19.sh for the maintained driver
cat n19jobs.txt | xargs -P 4 -L 1 bash -c 'timeout 5400 ./gt2p search 19 9 $0 1 $1 $2 1 > logs/n19/k$0.log 2>&1; echo "exit=$?" >> logs/n19/k$0.log'
echo ALLDONE > logs/n19/ALLDONE
