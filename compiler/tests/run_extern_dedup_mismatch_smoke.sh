#!/usr/bin/env bash
# TRACK_FFI_EXTERN_DEDUP STEP=4 — conflicting arity → E090.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURES="$COMPILER_DIR/tests/fixtures/extern_dedup"

expect_e090() {
  local file="$1"
  local output
  output="$("$MLCC" --check-only "$file" 2>&1 || true)"
  echo "$output" | grep -q 'error\[E090\]' || {
    echo "FAIL expected error[E090] in: $file" >&2
    echo "$output" >&2
    exit 1
  }
  echo "$output" | grep -q 'conflicts with earlier arity' || {
    echo "FAIL expected earlier-site mention in: $file" >&2
    echo "$output" >&2
    exit 1
  }
}

expect_clean() {
  local file="$1"
  local output
  output="$("$MLCC" --check-only "$file" 2>&1 || true)"
  if echo "$output" | grep -q 'error\[E090\]'; then
    echo "FAIL unexpected E090 in: $file" >&2
    echo "$output" >&2
    exit 1
  fi
}

expect_e090 "$FIXTURES/mismatch.mlc"
expect_clean "$FIXTURES/redeclare.mlc"
echo "extern_dedup_mismatch_smoke: ok"
