#!/usr/bin/env bash
# Profiling orchestrator: phase report, RSS, optional scaling and gprof hints.
#
# Usage:
#   ./benchmarks/profile/run_profile.sh
#   SCALING=1 ./benchmarks/profile/run_profile.sh
#   GPROF=1 ./benchmarks/profile/run_profile.sh

set -e
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MLCC="$ROOT/compiler/out/mlcc"
MAIN="$ROOT/compiler/main.mlc"
OUT="$ROOT/.tmp_profile"
mkdir -p "$OUT/emit"

if [ ! -x "$MLCC" ]; then
  echo "mlcc missing: $MLCC (run compiler/build.sh first)" >&2
  exit 1
fi

echo "=== mlcc --profile (phase timing + RSS) ==="
"$MLCC" --profile -o "$OUT/emit" "$MAIN" 2>"$OUT/phases.txt"
cat "$OUT/phases.txt"

echo ""
echo "=== /usr/bin/time -v (peak RSS, page faults) ==="
/usr/bin/time -v "$MLCC" -o "$OUT/emit2" "$MAIN" 2>"$OUT/time.txt" >/dev/null || true
grep -E 'Maximum resident set size|User time|Elapsed|Page faults' "$OUT/time.txt" || cat "$OUT/time.txt"

if [ "${SCALING:-0}" = "1" ]; then
  echo ""
  echo "=== scaling probe (functions) ==="
  ruby "$ROOT/benchmarks/scaling/run_quadratic_probe.rb" functions 64 2048 6
fi

if [ "${GPROF:-0}" = "1" ]; then
  echo ""
  echo "=== gprof (manual rebuild with -pg) ==="
  echo "  mlcc -o $OUT/gprof_emit $MAIN"
  echo "  g++ -std=c++20 -pg -O2 -I $OUT/gprof_emit -I $ROOT/runtime/include \\"
  echo "    $OUT/gprof_emit/*.cpp $ROOT/runtime/src/core/*.cpp $ROOT/runtime/src/io/*.cpp \\"
  echo "    -o $OUT/mlcc_prof"
  echo "  $OUT/mlcc_prof -o $OUT/gprof_run $MAIN"
  echo "  gprof $OUT/mlcc_prof gmon.out | head -40"
fi

echo ""
echo "Artifacts: $OUT/"
echo "Record baseline: $ROOT/benchmarks/profile/record_baseline.sh"
