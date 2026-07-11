#!/usr/bin/env bash
# TRACK_LANG_ERROR_UNION STEP=3 — mlcc T!E parse smoke (--check-only).
set -euo pipefail
COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURES="$COMPILER_DIR/tests/fixtures/error_union_sugar"

ok_output="$("$MLCC" --check-only "$FIXTURES/ok.mlc" 2>&1 || true)"
if echo "$ok_output" | grep -qiE 'error\[E|parse:'; then
  echo "FAIL unexpected error in ok.mlc:" >&2
  echo "$ok_output" >&2
  exit 1
fi

bad_output="$("$MLCC" --check-only "$FIXTURES/bang_missing_error.mlc" 2>&1 || true)"
echo "$bad_output" | grep -q 'expected type after ! in T!E' || {
  echo "FAIL expected T!E parse error in bang_missing_error.mlc:" >&2
  echo "$bad_output" >&2
  exit 1
}

echo "error_union_sugar_smoke: ok"
