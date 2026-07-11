#!/usr/bin/env bash
# TRACK_FFI_SAFETY STEP=2 — W-EXTERN-ATTR warning smoke (mlcc --check-only).
set -euo pipefail
COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURES="$COMPILER_DIR/tests/fixtures/extern_concurrency_attr"

expect_warning() {
  local file="$1"
  local output
  output="$("$MLCC" --check-only "$file" 2>&1 || true)"
  echo "$output" | grep -q 'warning\[W-EXTERN-ATTR\]' || {
    echo "FAIL expected W-EXTERN-ATTR in: $file" >&2
    echo "$output" >&2
    exit 1
  }
}

expect_clean() {
  local file="$1"
  local output
  output="$("$MLCC" --check-only "$file" 2>&1 || true)"
  if echo "$output" | grep -q 'warning\[W-EXTERN-ATTR\]'; then
    echo "FAIL unexpected W-EXTERN-ATTR in: $file" >&2
    echo "$output" >&2
    exit 1
  fi
}

expect_warning "$FIXTURES/missing_fn.mlc"
expect_warning "$FIXTURES/missing_type.mlc"
expect_clean "$FIXTURES/with_blocking.mlc"
expect_clean "$FIXTURES/shorthand.mlc"
echo "extern_concurrency_attr_smoke: ok"
