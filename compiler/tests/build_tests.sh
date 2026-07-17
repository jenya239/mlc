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

RUN_TESTS="$OUT_DIR/run_tests"
# Same Decision as build_tests_fast.sh / TRACK_CODEGEN_CPPAST_ONLY test-fix:
# Ruby ModularCompiler cannot rebuild the tests_main graph (cross-module
# namespaces). Keep existing run_tests; warn if stale. Full mlcc rebuild:
# compiler/tests/build_tests_self.sh (import-path WIP).
if [ ! -x "$RUN_TESTS" ]; then
  echo "[compiler tests] FAIL: missing $RUN_TESTS" >&2
  echo "[compiler tests] build with: compiler/tests/build_tests_self.sh" >&2
  exit 1
fi
if find "$COMPILER_DIR/tests" \( -name '*.mlc' -o -name '*.rb' \) -newer "$RUN_TESTS" -print -quit 2>/dev/null | grep -q .; then
  echo "[compiler tests] WARNING: tests newer than run_tests — running stale binary (Ruby rebuild disabled)" >&2
fi

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
