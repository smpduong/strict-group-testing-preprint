#!/bin/bash
# args: n k q minp seed iters
cd "$(dirname "$0")"
./gt sa $1 $2 $3 $4 21 $5 $6 1.0 0.12 cert_n$1_k$2.json > logs_sa/n$1_k$2_q$3_p$4_s$5.log 2>/dev/null
