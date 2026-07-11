#!/usr/bin/env bash
# TRACK_FFI_SAFETY STEP=3 — W-EXTERN-ARITY smoke (mlcc --check-only).
set -euo pipefail
COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURES="$COMPILER_DIR/tests/fixtures/extern_header_arity"

expect_warning() {
  local file="$1"
  local output
  output="$("$MLCC" --check-only "$file" 2>&1 || true)"
  echo "$output" | grep -q 'warning\[W-EXTERN-ARITY\]' || {
    echo "FAIL expected W-EXTERN-ARITY in: $file" >&2
    echo "$output" >&2
    exit 1
  }
}

expect_clean_arity() {
  local file="$1"
  local output
  output="$("$MLCC" --check-only "$file" 2>&1 || true)"
  if echo "$output" | grep -q 'warning\[W-EXTERN-ARITY\]'; then
    echo "FAIL unexpected W-EXTERN-ARITY in: $file" >&2
    echo "$output" >&2
    exit 1
  fi
}

expect_warning "$FIXTURES/mismatch.mlc"
expect_clean_arity "$FIXTURES/match_ok.mlc"
expect_clean_arity "$FIXTURES/no_import.mlc"
echo "extern_header_arity_smoke: ok"
