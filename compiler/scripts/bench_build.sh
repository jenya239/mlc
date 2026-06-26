#!/usr/bin/env bash
# Benchmark mlcc C++ build/link (TRACK_BUILD_SPEED). Fast by default.
# Usage: compiler/scripts/bench_build.sh
# BENCH_COLD=1 — also run MLCC_OBJ_CLEAN=1 build_bin (slow).
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
OUT_DIR="$COMPILER_DIR/out"
BINARY_OUT="$OUT_DIR/mlcc"

if [ ! -x "$BINARY_OUT" ]; then
  echo "bench_build: missing $BINARY_OUT — run compiler/build.sh first" >&2
  exit 1
fi

measure_seconds() {
  local output
  if ! output="$(/usr/bin/time -f '%e' "$@" 2>&1)"; then
    echo "$output" >&2
    return 1
  fi
  echo "$output" | tail -1
}

echo "bench_build: OUT=$OUT_DIR MLC_CXX=${MLC_CXX:-auto} MLCC_PCH=${MLCC_PCH:-1}"

touch "$COMPILER_DIR/build.sh"
build_sh_skip_sec="$(measure_seconds "$COMPILER_DIR/build.sh")"
echo "build_sh_skip_sec=${build_sh_skip_sec}"

build_bin_warm_sec="$(measure_seconds "$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BINARY_OUT")"
echo "build_bin_warm_sec=${build_bin_warm_sec}"

generated_cpp_count="$(find "$OUT_DIR" -maxdepth 1 -name '*.cpp' | wc -l)"
echo "generated_cpp_count=${generated_cpp_count}"

if [ "${BENCH_COLD:-0}" = "1" ]; then
  build_bin_cold_sec="$(measure_seconds env MLCC_OBJ_CLEAN=1 "$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "${BINARY_OUT}.bench_cold")"
  echo "build_bin_cold_sec=${build_bin_cold_sec}"
  rm -f "${BINARY_OUT}.bench_cold"
fi
