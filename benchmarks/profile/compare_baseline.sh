#!/usr/bin/env bash
# Compare current profile against recorded baseline.
#
# Usage: ./benchmarks/profile/compare_baseline.sh [baseline_file] [regression_factor]
# Default baseline: benchmarks/profile/baseline_reference.txt, factor: 1.15

set -e
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MLCC="$ROOT/compiler/out/mlcc"
MAIN="$ROOT/compiler/main.mlc"
BASELINE="${1:-$ROOT/benchmarks/profile/baseline_reference.txt}"
FACTOR="${2:-1.15}"
OUT="$ROOT/.tmp_profile"
SCALING_SOFT="${SCALING_SOFT_EXP:-1.35}"

if [ ! -x "$MLCC" ]; then
  echo "mlcc missing: $MLCC" >&2
  exit 1
fi
if [ ! -f "$BASELINE" ]; then
  echo "baseline missing: $BASELINE (run record_baseline.sh first)" >&2
  exit 1
fi

mkdir -p "$OUT/emit_cmp"
"$MLCC" --profile -o "$OUT/emit_cmp" "$MAIN" 2>"$OUT/phases_cmp.txt"

baseline_total=$(grep '^total_ms=' "$BASELINE" | cut -d= -f2)
current_total=$(grep '^total ' "$OUT/phases_cmp.txt" | awk '{print $2}')

echo "=== compare (factor=$FACTOR) ==="
echo "baseline total_ms=$baseline_total"
echo "current  total_ms=$current_total"

ruby -e "
baseline = Float('$baseline_total')
current = Float('$current_total')
factor = Float('$FACTOR')
limit = baseline * factor
if current > limit
  warn \"FAIL: total #{current}ms > baseline*#{factor} (#{limit}ms)\"
  exit 1
end
puts \"OK: total within #{factor}x baseline\"
"

scaling_out=$(ruby "$ROOT/benchmarks/scaling/run_quadratic_probe.rb" functions 100 800 5 2>&1)
echo "$scaling_out" | grep -E 'fitted|OK:|HARD' || true
b=$(echo "$scaling_out" | grep 'fitted exponent' | sed -n 's/.*b = \([0-9.]*\).*/\1/p')
if [ -n "$b" ]; then
  ruby -e "
b = Float('$b')
soft = Float('$SCALING_SOFT')
if b > soft
  warn \"FAIL: scaling exponent b=#{b} > #{soft}\"
  exit 1
end
puts \"OK: scaling b=#{b} <= #{soft}\"
"
fi

echo "compare_baseline: PASS"
