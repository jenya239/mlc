#!/usr/bin/env bash
# Tier A: run_tests + arch lint; skip fuzz/LSP. Reuse out/tests/run_tests when fresh.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out/tests}"
MLCC_BIN="$COMPILER_DIR/out/mlcc"
RUN_TESTS="$OUT_DIR/run_tests"
TESTS_MAIN="$COMPILER_DIR/tests/tests_main.mlc"
mkdir -p "$OUT_DIR"

need_compile=1
if [ -x "$RUN_TESTS" ]; then
  if ! find "$COMPILER_DIR/tests" \( -name '*.mlc' -o -name '*.rb' \) -newer "$RUN_TESTS" -print -quit 2>/dev/null | grep -q .; then
    need_compile=0
  fi
fi

if [ "$need_compile" -eq 1 ]; then
  echo "[fast] compile tests_main.mlc -> ${OUT_DIR}/run_tests" >&2
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
else
  echo "[fast] run_tests up to date - skip compile" >&2
fi

echo "[fast] run_tests" >&2
"$RUN_TESTS"

if [ -x "$MLCC_BIN" ]; then
  echo "[fast] mlcc --check-only main.mlc" >&2
  "$MLCC_BIN" --check-only "$COMPILER_DIR/main.mlc"
fi

echo "[fast] architecture lint" >&2
bash "$COMPILER_DIR/tests/run_architecture_lint.sh"
