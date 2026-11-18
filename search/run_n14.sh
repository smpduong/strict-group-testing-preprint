#!/bin/bash
# args: k q minp maxp seed
cd "$(dirname "$0")"
./gt sa 14 $1 $2 $3 $4 $5 15000000 1.0 0.12 cert_n14_k$1_s$5.json > logs/n14_k$1_q$2_p$3-$4_s$5.log 2>/dev/null
