#!/usr/bin/env bash
# Compile+run Validate runtime smoke (TRACK_STDLIB_VALIDATION STEP=2).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INCLUDE="$ROOT/runtime/include"
SOURCE="$ROOT/runtime/test/test_validate.cpp"
OUT="$ROOT/runtime/test/test_validate"
CXX="${CXX:-g++}"

"$CXX" -std=c++20 "-I${INCLUDE}" -o "$OUT" "$SOURCE"
"$OUT"
echo "validate runtime smoke OK"
