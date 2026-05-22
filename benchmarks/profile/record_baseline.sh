#!/usr/bin/env bash
# Record median phase profile + scaling exponent for before/after comparison.
#
# Usage: ./benchmarks/profile/record_baseline.sh [repeats]
# Output: .tmp_profile/baseline.txt

set -e
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MLCC="$ROOT/compiler/out/mlcc"
MAIN="$ROOT/compiler/main.mlc"
OUT="$ROOT/.tmp_profile"
REPEATS="${1:-3}"

if [ ! -x "$MLCC" ]; then
  echo "mlcc missing: $MLCC (run compiler/build.sh first)" >&2
  exit 1
fi

mkdir -p "$OUT/emit"

median() {
  ruby -e '
    values = ARGV.map(&:to_f).sort
    mid = values.length / 2
    puts values.length.odd? ? values[mid] : (values[mid - 1] + values[mid]) / 2.0
  ' "$@"
}

echo "Recording baseline ($REPEATS runs)..." >&2
phase_files=()
index=0
while [ "$index" -lt "$REPEATS" ]; do
  emit_dir="$OUT/baseline_emit_$index"
  mkdir -p "$emit_dir"
  "$MLCC" --profile -o "$emit_dir" "$MAIN" 2>"$OUT/phases_$index.txt"
  phase_files+=("$OUT/phases_$index.txt")
  index=$((index + 1))
done

{
  echo "# mlcc profile baseline $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "# repeats=$REPEATS main=$MAIN"
  echo ""
  echo "=== phases (median ms from last run table) ==="
  # Use last run for phase breakdown; wall total as median across runs
  tail -n +4 "${phase_files[$((REPEATS - 1))]}" | grep -E '^[a-z_]' || true
  echo ""
  echo "=== wall ms (median of $REPEATS runs) ==="
  wall_samples=()
  index=0
  while [ "$index" -lt "$REPEATS" ]; do
    total_line=$(grep '^total ' "$OUT/phases_$index.txt" | awk '{print $2}')
    wall_samples+=("$total_line")
    index=$((index + 1))
  done
  echo "total_ms=$(median "${wall_samples[@]}")"
  echo ""
  echo "=== scaling (functions 100..800) ==="
  ruby "$ROOT/benchmarks/scaling/run_quadratic_probe.rb" functions 100 800 5 2>&1 | grep -E 'fitted|OK:|HARD' || true
} | tee "$OUT/baseline.txt"

echo "Wrote $OUT/baseline.txt" >&2
