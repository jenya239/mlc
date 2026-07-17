#!/usr/bin/env bash
# Tier A: run_tests + arch lint; skip fuzz/LSP.
# Do NOT rebuild run_tests via Ruby ModularCompiler here: the tests_main graph
# pulls in the whole self-hosted compiler, and Ruby→C++ emission currently
# fails cross-module namespaces (see TRACK_CODEGEN_CPPAST_ONLY test-fix).
# Full rebuild: compiler/tests/build_tests_self.sh (mlcc). Tier B
# (`build_tests.sh`) also skips Ruby phase 1 under the same Decision.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out/tests}"
MLCC_BIN="$COMPILER_DIR/out/mlcc"
RUN_TESTS="$OUT_DIR/run_tests"
mkdir -p "$OUT_DIR"

if [ ! -x "$RUN_TESTS" ]; then
  echo "[fast] FAIL: missing $RUN_TESTS" >&2
  echo "[fast] build with: compiler/tests/build_tests_self.sh   # or build_tests.sh" >&2
  exit 1
fi

if find "$COMPILER_DIR/tests" \( -name '*.mlc' -o -name '*.rb' \) -newer "$RUN_TESTS" -print -quit 2>/dev/null | grep -q .; then
  echo "[fast] WARNING: compiler/tests newer than run_tests — running stale binary (Ruby rebuild disabled)" >&2
fi

echo "[fast] run_tests" >&2
"$RUN_TESTS"

if [ -x "$MLCC_BIN" ]; then
  echo "[fast] mlcc --check-only main.mlc" >&2
  "$MLCC_BIN" --check-only "$COMPILER_DIR/main.mlc"
fi

echo "[fast] architecture lint" >&2
bash "$COMPILER_DIR/tests/run_architecture_lint.sh"
