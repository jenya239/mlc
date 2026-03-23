#!/usr/bin/env bash
# Run compiler unit tests using the self-hosted mlcc binary.
# Usage: build_tests_self.sh [mlcc_binary] [out_dir]
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
OUT_DIR="${2:-/tmp/tests_self_$$}"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC="$ROOT_DIR/runtime/src/io/io.cpp $ROOT_DIR/runtime/src/core/string.cpp"

mkdir -p "$OUT_DIR"

echo "Compiling tests with $MLCC ..."
"$MLCC" -o "$OUT_DIR" "$COMPILER_DIR/tests/tests_main.mlc"

echo "Compiling C++ ..."
g++ -std=c++20 -I "$OUT_DIR" -I "$RT_INC" "$OUT_DIR"/*.cpp $RT_SRC -o "$OUT_DIR/run_tests"

echo "Running tests..."
"$OUT_DIR/run_tests"
