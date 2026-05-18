#!/usr/bin/env bash
# Runs synthetic benchmarks comparing mlcc and Ruby translation speed.
# Usage: ./run_bench.sh [suite] [scales...]
# Example: ./run_bench.sh functions 50 100 200 500

set -e
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MLCC="$ROOT/compiler/out/mlcc"
GEN="$(dirname "$0")/gen.rb"

SUITE="${1:-functions}"
shift
SCALES="${*:-50 100 200 500}"

RUBY_BENCH_SCRIPT="$(mktemp /tmp/ruby_bench_XXXX.rb)"
cat > "$RUBY_BENCH_SCRIPT" << 'RUBY'
$LOAD_PATH.unshift(ARGV[2])
require "mlc/common/index"
require "mlc/common/modular_compilation/modular_compiler"
out_dir = ARGV[1]
FileUtils.mkdir_p(out_dir)
compiler = MLC::Common::ModularCompilation::ModularCompiler.new(
  entry_path: ARGV[0],
  out_dir: out_dir,
  root_dir: File.dirname(ARGV[0]),
  binary_name: "bench"
)
t0 = Process.clock_gettime(Process::CLOCK_MONOTONIC)
compiler.compile
t1 = Process.clock_gettime(Process::CLOCK_MONOTONIC)
puts ((t1 - t0) * 1000).round
RUBY

printf "%-10s %8s %10s %10s %8s\n" "suite" "scale" "mlcc(ms)" "ruby(ms)" "ratio"
printf "%-10s %8s %10s %10s %8s\n" "----------" "--------" "----------" "----------" "--------"

for SCALE in $SCALES; do
  GEN_DIR="$(mktemp -d /tmp/bench_gen_XXXX)"
  ruby "$GEN" "$SUITE" "$SCALE" "$GEN_DIR" > /dev/null

  MLCC_OUT="$(mktemp -d /tmp/bench_mlcc_XXXX)"
  RUBY_OUT="$(mktemp -d /tmp/bench_ruby_XXXX)"

  T0=$(date +%s%N)
  "$MLCC" -o "$MLCC_OUT" "$GEN_DIR/main.mlc" 2>/dev/null
  T1=$(date +%s%N)
  MLCC_MS=$(( (T1 - T0) / 1000000 ))
  RUBY_MS=$( bundle exec ruby "$RUBY_BENCH_SCRIPT" "$GEN_DIR/main.mlc" "$RUBY_OUT" "$ROOT/lib" 2>/dev/null )

  RATIO=$(echo "scale=1; $RUBY_MS / $MLCC_MS" | bc 2>/dev/null || echo "?")

  printf "%-10s %8d %10d %10d %7sx\n" "$SUITE" "$SCALE" "$MLCC_MS" "$RUBY_MS" "$RATIO"

  rm -rf "$GEN_DIR" "$MLCC_OUT" "$RUBY_OUT"
done

rm -f "$RUBY_BENCH_SCRIPT"
