#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

TESTS_MAIN="$COMPILER_DIR/tests/tests_main.mlc"

echo "[compiler tests] phase 1/6: compile tests_main.mlc → ${OUT_DIR}/run_tests" >&2

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

echo "[compiler tests] phase 2/6: execute run_tests" >&2
"$OUT_DIR/run_tests"

echo "[compiler tests] phase 3/6: fuzz smoke (mlcc --check-only)" >&2
bash "$COMPILER_DIR/tests/fuzz/run_fuzz_smoke.sh" "$COMPILER_DIR/out/mlcc"

echo "[compiler tests] phase 4/6: negative corpus (mlcc --check-only)" >&2
bash "$COMPILER_DIR/tests/fuzz/run_negative_corpus.sh" "$COMPILER_DIR/out/mlcc"

echo "[compiler tests] phase 5/6: fuzz differential (mlcc vs Ruby checker)" >&2
bash "$COMPILER_DIR/tests/fuzz/run_fuzz_differential.sh" "$COMPILER_DIR/out/mlcc"

echo "[compiler tests] phase 6/7: compile_commands smoke (mlcc --emit-compile-commands)" >&2
bash "$COMPILER_DIR/tests/run_compile_commands_smoke.sh" "$COMPILER_DIR/out/mlcc"

echo "[compiler tests] phase 7/7: lsp smoke (mlcc lsp stdio)" >&2
MLCC_FORCE_RUBY=1 bash "$COMPILER_DIR/build.sh" "$OUT_DIR" >/dev/null
bash "$COMPILER_DIR/tests/run_lsp_smoke.sh" "$OUT_DIR/mlcc"
