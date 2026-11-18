#!/bin/bash
# reproduce_n19.sh - build and run the slices of the t(19,2,2)=10 proof (and the n=14, T=8 prerequisite f(8)=13).
# Everything is relative to the repository; nothing is written outside it.
#
#   bash reproduce_n19.sh list                 show slices with time estimates
#   bash reproduce_n19.sh quick                run the slices that finish in minutes
#   bash reproduce_n19.sh <slice> [<slice>..]  run chosen slices
#   bash reproduce_n19.sh long                 run the slices that take > 30 min (hours)
#   bash reproduce_n19.sh manifest             (re)generate repro_logs/n19/MANIFEST.txt from the logs present
#   bash reproduce_n19.sh stored               hash + classify the committed historical logs (no runs)
#
# Env: RUN_TIMEOUT (seconds per run, default 2700 = 45 min), JOBS (parallel runs, default 2), GT_NODELIMIT (see below).
# A timed-out run is recorded as "TIMEOUT" and is INCOMPLETE: never use it as evidence.
# Node limit: the CSP solver (search/gt.c) stops a single r(W) check after GT_NODELIMIT nodes (default 2,000,000,000)
# and then reports UNKNOWN; any such event makes the run print "STATUS: INCOMPLETE". The colouring solvers
# (independent/gt2*.c) and cf.c have no node limit.
set -u
cd "$(dirname "$0")"
ROOT=$PWD
BUILD=$ROOT/repro_build
LOGS=$ROOT/repro_logs/n19
mkdir -p "$BUILD" "$LOGS"
CFLAGS="-O2"
RUN_TIMEOUT=${RUN_TIMEOUT:-2700}
JOBS=${JOBS:-2}

# slice name | binary (built from) | command line | env | approx time (this machine, 1 core)
declare -A CMD ENVV EST SRC
def() { CMD[$1]="$3"; ENVV[$1]="$4"; EST[$1]="$5"; SRC[$1]="$2"; }
# n = 14, T = 8 (gives f(8)=13, used by Lemma A for n = 19)
def n14_k1 search/exh2.c  "exh2w 14 1 7 0 14" ""              "<1 s"
def n14_k2 search/exh2.c  "exh2w 14 2 6 0 14" ""              "<1 s"
def n14_k3 search/exh2.c  "exh2 14 3 5 0 14"  ""              "2 s"
def n14_k4 search/exh2.c  "exh2 14 4 4 0 14"  ""              "~10 s"
def n14_k5 search/exh2.c  "exh2 14 5 3 0 14"  ""              "~1 min"
def n14_k6 search/exh2.c  "exh2 14 6 2 0 14"  ""              "~10-20 min"
def n14_k7 search/exh2.c  "exh2 14 7 1 0 14"  ""              "~30-60 min"
def n14_k8 search/exh2.c  "exh2 14 8 0 0 14"  ""              "~15-30 min"
def n14_k5_gt2 independent/gt2.c "gt2 search 14 8 5" ""        "~10 s"
def n14_k6_gt2 independent/gt2.c "gt2 search 14 8 6" ""        "~1 min"
def n14_k7_gt2 independent/gt2.c "gt2 search 14 8 7" ""        "~5 min"
def n14_k8_gt2 independent/gt2.c "gt2 search 14 8 8" ""        "~3 min"
# n = 19, T = 9
def n19_k5_gt2p  independent/gt2p.c "gt2p search 19 9 5 0 0 19 1" ""          "157 s (stored log)"
def n19_k6_exh7  search/exh7.c      "exh7 19 6 3 6 6"             "INTERCAP=2" "~10 s"
def n19_k7_exh7  search/exh7.c      "exh7 19 7 2 6 9"             ""           "~10-30 min"
def n19_k7_gt2p  independent/gt2p.c "gt2p search 19 9 7 0 6 9 1"  ""           "308 s (stored log)"
def n19_k8_gt2p  independent/gt2p.c "gt2p search 19 9 8 0 6 9 1"  ""           "811 s (stored log)"
def n19_k5_exh2  search/exh2.c      "exh2 19 5 4 0 19"            ""           "unknown, est. 20-60 min"
def n19_k8_exh7  search/exh7.c      "exh7 19 8 1 6 9"             ""           "unknown, est. 30-90 min (195M r(W) calls)"
def aud_n19_k6   independent/auditor/mysearch.cpp "mysearch 19 6 3 6 6 2" ""     "~2 min (from file times)"
def aud_n19_k7   independent/auditor/mysearch.cpp "mysearch 19 7 2 6 9"   ""     "~2-5 min (from file times)"
def aud_n19_k8   independent/auditor/mysearch.cpp "mysearch 19 8 1 6 9"   ""     "~5-15 min (from file times)"
def aud_cff_9_13 independent/auditor/mycff.cpp    "mycff 9 13"            ""     "~1 min (29.9M nodes)"
def aud_n19_k5   independent/auditor/mysearch.cpp "mysearch 19 5 4 0 19"  ""     "NOT FINISHED by the auditor (killed at ~28 min): INCOMPLETE; not run here"
def n19_k6_gt2p  independent/gt2p.c "gt2p search 19 9 6 0 6 6 1"  ""           "~4 s (pools 6..6); the 5362 s stored log is the no-pool-bound run"
def n19_k9_cf    search/cf.c        "cf 13 9"                     ""           "781 s (647M nodes)"

QUICK="aud_n19_k6 aud_n19_k7 aud_cff_9_13 n14_k1 n14_k2 n14_k3 n14_k4 n14_k5 n14_k5_gt2 n14_k6_gt2 n14_k7_gt2 n14_k8_gt2 n19_k6_exh7 n19_k5_gt2p n19_k7_gt2p n19_k8_gt2p"
MEDIUM="aud_n19_k8 n14_k6 n14_k7 n14_k8 n19_k7_exh7 n19_k5_exh2 n19_k8_exh7"
LONG="n19_k6_gt2p n19_k9_cf"
# aud_n19_k5 is listed (list) but never run by a group: the auditor's own k=5 run is INCOMPLETE (killed); run by name only.

build() { # build <source> : binary named after the command's first word
  local s=$1 b
  case $s in
    search/exh2.c) gcc $CFLAGS -o "$BUILD/exh2" "$s" -lm && gcc $CFLAGS -DWIDE -o "$BUILD/exh2w" "$s" -lm ;;
    search/cf.c)   gcc $CFLAGS -o "$BUILD/cf" "$s" ;;
    search/exh7.c) gcc $CFLAGS -o "$BUILD/exh7" "$s" -lm ;;
    independent/gt2.c)  gcc $CFLAGS -o "$BUILD/gt2" "$s" ;;
    independent/gt2p.c) gcc $CFLAGS -o "$BUILD/gt2p" "$s" ;;
    independent/auditor/mysearch.cpp) g++ $CFLAGS -std=c++17 -o "$BUILD/mysearch" "$s" ;;
    independent/auditor/mycff.cpp)    g++ $CFLAGS -std=c++17 -o "$BUILD/mycff" "$s" ;;
    *) echo "unknown source $s"; return 1 ;;
  esac
}

run_one() {
  local name=$1 log="$LOGS/$1.log" t0 rc
  t0=$(date +%s)
  # shellcheck disable=SC2086
  ( cd "$BUILD" && env ${ENVV[$name]} timeout "$RUN_TIMEOUT" ./${CMD[$name]} ) > "$log" 2> "$log.err"
  rc=$?
  { echo "exit=$rc"; echo "elapsed_s=$(( $(date +%s) - t0 ))"; } >> "$log"
  [ $rc -eq 124 ] && echo "TIMEOUT: run stopped after ${RUN_TIMEOUT}s, INCOMPLETE" >> "$log"
  echo "$name: exit=$rc  $(grep -h '^STATUS' "$log" | head -1)"
}

sha() { sha256sum "$1" | cut -d' ' -f1; }

status_of() { # status_of <log>
  local f=$1
  if grep -q '^TIMEOUT' "$f"; then echo "INCOMPLETE (timeout)"; return; fi
  local s; s=$(grep -h '^STATUS' "$f" | head -1)
  if [ -n "$s" ]; then echo "${s#STATUS: }"; return; fi
  if grep -q 'complete=1\|^DONE\|exists=' "$f"; then echo "finished (log predates the STATUS line; DONE/RESULT line present)"; else echo "NO COMPLETION LINE (incomplete or killed)"; fi
}

manifest() {
  local M="$LOGS/MANIFEST.txt"
  {
  echo "# MANIFEST for n=19 reproduction (generated by reproduce_n19.sh manifest, $(date -u +%Y-%m-%dT%H:%M:%SZ))"
  echo "git commit: $(git rev-parse HEAD 2>/dev/null) $(git diff --quiet 2>/dev/null || echo '(+ uncommitted changes)')"
  echo "gcc: $(gcc --version | head -1)"
  echo "compile flags: $CFLAGS (auditor files: g++ -O2 -std=c++17; g++ $(g++ --version | head -1))   (exh2w = same source with -DWIDE; link -lm for the search/ programs)"
  echo "node limit: search/gt.c CSP = ${GT_NODELIMIT:-2000000000} nodes per r(W) check (env GT_NODELIMIT); gt2*.c and cf.c: none"
  echo "per-run timeout: ${RUN_TIMEOUT}s"
  echo
  echo "## source files (sha256)"
  for f in search/gt.c search/exh2.c search/exh7.c search/cf.c independent/gt2.c independent/gt2p.c independent/auditor/core.hpp independent/auditor/mysearch.cpp independent/auditor/mycff.cpp; do echo "$(sha $f)  $f"; done
  echo
  echo "## runs (slice | command | completion flag | log sha256)"
  for name in "${!CMD[@]}"; do
    [ -f "$LOGS/$name.log" ] || continue
    echo "$name | cd repro_build && ${ENVV[$name]} ./${CMD[$name]} | $(status_of "$LOGS/$name.log") | $(sha "$LOGS/$name.log")  $(grep -h '^DONE\|^RESULT' "$LOGS/$name.log" | head -1 | cut -c1-170)"
  done | sort
  } > "$M"
  echo "wrote $M"
}

stored() {
  echo "## committed historical logs (sha256 | completion)"
  for f in search/logs/exh2_14_*p0-14.log search/logs/exh2w_14_*.log search/logs/exh2_19_5_4_p0.log search/logs19/n19_k*_p6-*.log search/logs19x/*.log independent/logs/n19full/k*.log independent/logs/n19top/k*.log n29/logs/n19_k*.log n29/logs/n14_k*.log; do
    [ -f "$f" ] && echo "$(sha $f)  $f | $(status_of $f)"
  done
}

case "${1:-list}" in
  list)
    for n in $QUICK $MEDIUM $LONG aud_n19_k5; do printf '%-14s %-34s %-12s est: %s\n' "$n" "${CMD[$n]}" "${ENVV[$n]}" "${EST[$n]}"; done
    echo "groups: quick = $QUICK"; echo "        medium = $MEDIUM"; echo "        long = $LONG" ;;
  manifest) manifest ;;
  stored) stored ;;
  quick|medium|long|all)
    case $1 in quick) L=$QUICK;; medium) L=$MEDIUM;; long) L=$LONG;; all) L="$QUICK $MEDIUM $LONG";; esac
    # build everything first (serially), then run JOBS at a time
    for n in $L; do build "${SRC[$n]}" || exit 1; done
    for n in $L; do
      while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do sleep 2; done
      run_one "$n" & done; wait
    manifest ;;
  *)
    for n in "$@"; do [ -n "${CMD[$n]:-}" ] || { echo "unknown slice $n (try: list)"; exit 1; }; done
    for n in "$@"; do build "${SRC[$n]}" || exit 1; done
    for n in "$@"; do
      while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do sleep 2; done
      run_one "$n" & done; wait
    manifest ;;
esac
