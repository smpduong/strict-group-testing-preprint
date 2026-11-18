#!/bin/bash
# Tests for the work partitioning of independent/gt2p.c (GT2P_P, GT2P_PART, GT2P_SPLITD).
# Run: bash tests/test_gt2p_partition.sh   (fast, ~15 s).  GT2P_SLOW=1 adds n=14 k=6 and n=19 (many minutes).
# Pure-partition argument checked here: every depth-SPLITD node gets index c (0,1,2,..) in DFS order, identical in all parts
# because the tree above SPLITD is walked by every part; part p keeps c%P==p. So each node at depth>=SPLITD lies below
# exactly one kept node (summed nodes/checks/colour_*/designs equal the unpartitioned run), and nodes at depth<SPLITD are
# counted in a separate "top" counter that is added to the reported totals in part 0 only.
cd "$(dirname "$0")/.." || exit 1
T=$(mktemp -d "${TMPDIR:-/tmp}/gt2p-tests.XXXXXX"); trap 'rm -rf "$T"' EXIT
gcc -O2 -o "$T/gt2p" independent/gt2p.c || exit 1
fails=0
ok(){ echo "PASS $1"; }; bad(){ echo "FAIL $1"; fails=$((fails+1)); }
field(){ grep '^RESULT' "$1" | grep -o "$2=[0-9]*" | head -1 | cut -d= -f2; }

# sums_match "label" P splitd "search args..."  -> compares designs,nodes,checks,colour_calls,colour_nodes, sum over parts vs P=1 run
sums_match() {
  local lab=$1 P=$2 D=$3; shift 3
  "$T/gt2p" search "$@" > "$T/full.out" 2>/dev/null
  local bd=0 res="" f
  for ((p=0;p<P;p++)); do GT2P_P=$P GT2P_PART=$p GT2P_SPLITD=$D "$T/gt2p" search "$@" > "$T/part$p.out" 2>/dev/null; done
  local good=1 nz=0
  for f in designs nodes checks colour_calls colour_nodes; do
    local s=0 want; want=$(field "$T/full.out" $f)
    for ((p=0;p<P;p++)); do s=$((s+$(field "$T/part$p.out" $f))); done
    [ "$s" = "$want" ] || { good=0; echo "   $f: sum=$s full=$want"; }
  done
  # every part sees the same number of split-level subtrees; top nodes agree; status complete; each part reports P/part/splitd
  local st; st=$(field "$T/part0.out" split_subtrees)
  for ((p=0;p<P;p++)); do
    [ "$(field "$T/part$p.out" split_subtrees)" = "$st" ] || good=0
    [ "$(field "$T/part$p.out" P)" = "$P" ] && [ "$(field "$T/part$p.out" part)" = "$p" ] && [ "$(field "$T/part$p.out" splitd)" = "$D" ] || good=0
    grep -q '^STATUS: COMPLETE' "$T/part$p.out" || good=0
  done
  [ "$good" = 1 ] && ok "$lab (P=$P splitd=$D, split_subtrees=$st, designs=$(field "$T/full.out" designs), nodes=$(field "$T/full.out" nodes))" || bad "$lab (P=$P splitd=$D)"
}

echo "== default behaviour unchanged"
"$T/gt2p" search 14 8 5 0 > "$T/d.out" 2>/dev/null
[ "$(field "$T/d.out" nodes)" = 334880 ] && [ "$(field "$T/d.out" designs)" = 0 ] && [ "$(field "$T/d.out" P)" = 1 ] && grep -q '^STATUS: COMPLETE' "$T/d.out" && ok "unset env: n=14 T=8 k=5 nodes=334880 designs=0 P=1" || bad "default run"

echo "== sums over parts == unpartitioned run"
sums_match "n=14 T=8 k=5" 3 4 14 8 5 0
sums_match "n=14 T=8 k=5" 7 3 14 8 5 0
sums_match "n=14 T=8 k=5" 5 6 14 8 5 0
sums_match "n=14 T=8 k=5, split depth 0 (single subtree)" 3 0 14 8 5 0
sums_match "n=18 T=9 k=6 pools 6..6 (cmd-line style args)" 4 4 18 9 6 0 6 6 1
echo "== positive controls: designs found exactly once overall"
sums_match "n=13 T=8 k=5 (exactly 1 design)" 4 4 13 8 5 0
n=0; for ((p=0;p<4;p++)); do [ "$(field "$T/part$p.out" designs)" = 1 ] && n=$((n+1)); done
[ $n = 1 ] && [ "$(field "$T/full.out" designs)" = 1 ] && ok "n=13: exactly one part found the design" || bad "n=13 exactly one part ($n)"
sums_match "n=12 T=8 k=5 (11 designs)" 5 4 12 8 5 0
[ "$(field "$T/full.out" designs)" = 11 ] && ok "n=12: 11 designs total" || bad "n=12 design count"

echo "== bad parameters rejected"
GT2P_P=3 GT2P_PART=3 "$T/gt2p" search 13 8 5 0 >/dev/null 2>&1 && bad "PART>=P accepted" || ok "PART>=P rejected"
GT2P_PROBE=1 GT2P_P=2 GT2P_PART=0 "$T/gt2p" search 14 8 5 0 > "$T/pr.out" 2>/dev/null
[ "$(field "$T/pr.out" split_subtrees)" -gt 0 ] && [ "$(field "$T/pr.out" designs)" = 0 ] && ok "GT2P_PROBE=1 counts split subtrees only" || bad "probe"

if [ -n "$GT2P_SLOW" ]; then
  echo "== slow: n=14 T=8 k=6, n=19 T=9 k=7 (13,504,468 nodes)"
  sums_match "n=14 T=8 k=6" 3 4 14 8 6 0
  sums_match "n=19 T=9 k=7 pools 6..9" 4 4 19 9 7 0 6 9 1
  [ "$(field "$T/full.out" nodes)" = 13504468 ] && ok "n=19 nodes=13504468" || bad "n=19 nodes"
fi
echo "== gt2p partition tests: $fails failure(s)"
exit $((fails>0))
