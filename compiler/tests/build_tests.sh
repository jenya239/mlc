#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out/tests}"
MLCC_OUT="$COMPILER_DIR/out"
MLCC_BIN="$MLCC_OUT/mlcc"
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
bash "$COMPILER_DIR/tests/fuzz/run_fuzz_smoke.sh" "$MLCC_BIN"

echo "[compiler tests] phase 4/6: negative corpus (mlcc --check-only)" >&2
bash "$COMPILER_DIR/tests/fuzz/run_negative_corpus.sh" "$MLCC_BIN"

echo "[compiler tests] phase 5/7: fuzz differential (mlcc vs Ruby checker)" >&2
bash "$COMPILER_DIR/tests/fuzz/run_fuzz_differential.sh" "$MLCC_BIN"

echo "[compiler tests] phase 6/8: cpp parser differential (mlcc vs Ruby CppAst)" >&2
bash "$COMPILER_DIR/tests/run_cpp_parser_differential.sh"

echo "[compiler tests] phase 7/8: compile_commands smoke (mlcc --emit-compile-commands)" >&2
bash "$COMPILER_DIR/tests/run_compile_commands_smoke.sh" "$MLCC_BIN"

echo "[compiler tests] phase 8/9: lsp smoke (mlcc lsp stdio)" >&2
if [ ! -x "$MLCC_BIN" ] || find "$COMPILER_DIR" -name '*.mlc' -newer "$MLCC_BIN" -print -quit 2>/dev/null | grep -q .; then
  echo "[compiler tests] mlcc stale/missing — rebuild via build.sh" >&2
  bash "$COMPILER_DIR/build.sh" "$MLCC_OUT" >/dev/null
else
  echo "[compiler tests] mlcc fresh — skip build.sh" >&2
fi
bash "$COMPILER_DIR/tests/run_lsp_smoke.sh" "$MLCC_BIN"

echo "[compiler tests] phase 9/10: vm vs C++ exit diff (mlcc --run)" >&2
if [ ! -x "$MLCC_BIN" ] || find "$COMPILER_DIR/vm" "$COMPILER_DIR/mir" -name '*.mlc' -newer "$MLCC_BIN" -print -quit 2>/dev/null | grep -q .; then
  echo "[compiler tests] mlcc stale for vm diff — rebuild via build.sh" >&2
  bash "$COMPILER_DIR/build.sh" "$MLCC_OUT" >/dev/null
fi
MLCC="$MLCC_BIN" bash "$COMPILER_DIR/tests/run_vm_cpp_exit_diff.sh"

echo "[compiler tests] phase 10/10: architecture lint" >&2
bash "$COMPILER_DIR/tests/run_architecture_lint.sh"
