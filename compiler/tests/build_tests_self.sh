#!/usr/bin/env bash
# Run compiler unit tests using the self-hosted mlcc binary.
# Usage: build_tests_self.sh [mlcc_binary] [out_dir]
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
OUT_DIR="${2:-/tmp/tests_self_$$}"
mkdir -p "$OUT_DIR"

echo "Compiling tests with $MLCC ..."
"$MLCC" -o "$OUT_DIR" "$COMPILER_DIR/tests/tests_main.mlc"

echo "Linking C++ ..."
MLCC_ENTRY_BASENAME=tests_main bash "$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$OUT_DIR/run_tests"

echo "Running tests..."
"$OUT_DIR/run_tests"
