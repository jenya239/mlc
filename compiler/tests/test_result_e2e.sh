#!/usr/bin/env bash
# E2E test: compile test_result_program.mlc with mlcc, run it, check output.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
TMP="$(mktemp -d)"
trap "rm -rf $TMP" EXIT

echo "=== E2E: Result + ? ==="

"$MLCC" "$COMPILER_DIR/tests/test_result_program.mlc" -o "$TMP"

# Runtime headers
RUNTIME="$COMPILER_DIR/../runtime/include"
g++ -std=c++20 -I"$RUNTIME" -I"$TMP" \
    "$TMP/test_result_program.cpp" \
    "$COMPILER_DIR/../runtime/src/io/io.cpp" \
    -o "$TMP/test_result_program"

OUTPUT="$("$TMP/test_result_program")"
EXPECTED="ok: 6"

if [ "$OUTPUT" = "$EXPECTED" ]; then
  echo "PASS: output = '$OUTPUT'"
else
  echo "FAIL: expected '$EXPECTED', got '$OUTPUT'"
  exit 1
fi
