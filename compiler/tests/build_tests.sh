#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

TESTS_MAIN="$COMPILER_DIR/tests/tests_main.mlc"

bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "mlc/common/index"
require "mlc/common/modular_compilation/modular_compiler"

compiler = MLC::Common::ModularCompilation::ModularCompiler.new(
  entry_path: ARGV[0],
  out_dir: ARGV[1],
  root_dir: File.dirname(ARGV[0]),
  binary_name: "run_tests"
)
result = compiler.build
puts "Built: #{result[:binary]}"
' "$TESTS_MAIN" "$OUT_DIR"

echo "Running tests..."
"$OUT_DIR/run_tests"
