#!/usr/bin/env bash
# TRACK_STDLIB_LOGIC_TO_MLC STEP=6 — C++ Env runtime only (Log is MLC).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INCLUDE="$ROOT/runtime/include"
SOURCE="$ROOT/runtime/test/test_env_log.cpp"
OUT="$ROOT/runtime/test/test_env_log"
CXX="${CXX:-g++}"

"$CXX" -std=c++20 "-I${INCLUDE}" -o "$OUT" "$SOURCE"
"$OUT"
echo "env_log runtime smoke OK"
