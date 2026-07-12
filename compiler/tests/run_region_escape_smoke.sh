#!/usr/bin/env bash
# TRACK_LANG_REGION_ARENA STEP=5 — region escape vectors must be compile errors (E091).
set -euo pipefail
COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
E2E="$COMPILER_DIR/tests/e2e"

expect_e091() {
  local file="$1"
  local needle="$2"
  local output
  local code=0
  output="$("$MLCC" --check-only "$file" 2>&1)" || code=$?
  if [ "$code" -eq 0 ]; then
    echo "FAIL expected non-zero exit for: $file" >&2
    echo "$output" >&2
    exit 1
  fi
  echo "$output" | grep -q 'error\[E091\]' || {
    echo "FAIL expected error[E091] in: $file" >&2
    echo "$output" >&2
    exit 1
  }
  echo "$output" | grep -q "$needle" || {
    echo "FAIL expected message containing '$needle' in: $file" >&2
    echo "$output" >&2
    exit 1
  }
  echo "ok $(basename "$file")"
}

expect_e091 "$E2E/region_escape_return.mlc" 'escapes via return'
expect_e091 "$E2E/region_escape_closure.mlc" 'closure capture'
expect_e091 "$E2E/region_escape_field.mlc" 'Region type not allowed'
echo "region_escape_smoke: ok"
