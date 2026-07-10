#!/usr/bin/env bash
# Compile+run WebSocket runtime smoke (TRACK_STDLIB_WEBSOCKET STEP=2).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INCLUDE="$ROOT/runtime/include"
SOURCE="$ROOT/runtime/test/test_websocket.cpp"
OUT="$ROOT/runtime/test/test_websocket"
CXX="${CXX:-g++}"

"$CXX" -std=c++20 -pthread "-I${INCLUDE}" -o "$OUT" "$SOURCE"
"$OUT"
echo "websocket runtime smoke OK"
