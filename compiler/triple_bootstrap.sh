#!/usr/bin/env bash
# Triple-bootstrap verification.
# Proves compiler stability: mlcc3 output == mlcc2 output.
#
# Usage: triple_bootstrap.sh [mlcc_binary] [work_dir]
#
# Steps:
#   1. mlcc  → /work/bs1/*.cpp → g++ → mlcc2
#   2. mlcc2 → /work/bs2/*.cpp → g++ → mlcc3
#   3. mlcc3 → /work/bs3/*.cpp
#   4. diff bs2/ bs3/ — must be empty
set -e

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
WORK_DIR="${2:-/tmp/mlc_triple_$$}"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC="$ROOT_DIR/runtime/src/io/io.cpp $ROOT_DIR/runtime/src/core/string.cpp"
ENTRY="$COMPILER_DIR/main.mlc"
CXX_FLAGS="-std=c++20 -I $RT_INC"

echo "=== Triple-bootstrap ==="
echo "mlcc:     $MLCC"
echo "work_dir: $WORK_DIR"
echo ""

build_gen() {
  local binary="$1"
  local out_dir="$2"
  mkdir -p "$out_dir"
  "$binary" -o "$out_dir" "$ENTRY"
}

build_bin() {
  local out_dir="$1"
  local binary="$2"
  g++ $CXX_FLAGS -I "$out_dir" "$out_dir"/*.cpp $RT_SRC -o "$binary"
}

echo "[1/4] mlcc → bs1 (C++ gen) ..."
build_gen "$MLCC" "$WORK_DIR/bs1"

echo "[2/4] g++ bs1 → mlcc2 ..."
build_bin "$WORK_DIR/bs1" "$WORK_DIR/mlcc2"

echo "[3/4] mlcc2 → bs2 (C++ gen) ..."
build_gen "$WORK_DIR/mlcc2" "$WORK_DIR/bs2"

echo "[4/4] g++ bs2 → mlcc3 ..."
build_bin "$WORK_DIR/bs2" "$WORK_DIR/mlcc3"

echo "[5/5] mlcc3 → bs3 (C++ gen) ..."
build_gen "$WORK_DIR/mlcc3" "$WORK_DIR/bs3"

echo ""
echo "=== Diff bs2 vs bs3 ==="
if diff -rq --exclude="*.o" "$WORK_DIR/bs2" "$WORK_DIR/bs3"; then
  echo "PASS: bs2 == bs3, compiler is stable"
else
  echo "FAIL: bs2 != bs3, compiler is not yet stable"
  diff -r --exclude="*.o" "$WORK_DIR/bs2" "$WORK_DIR/bs3" | head -40
  exit 1
fi
