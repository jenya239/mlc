#!/usr/bin/env bash
# Fast parallel build of a directory of .cpp files into a binary.
# Usage: build_bin.sh <cpp_dir> <binary_out> [extra_include_dir]
# Respects MLC_CXX (default: ccache clang++ > ccache g++ > g++) and MLC_JOBS.
set -e

CPP_DIR="$1"
BIN_OUT="$2"
EXTRA_INC="${3:-}"

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC=(
  "$ROOT_DIR/runtime/src/io/io.cpp"
  "$ROOT_DIR/runtime/src/core/string.cpp"
  "$ROOT_DIR/runtime/src/core/profile.cpp"
)

JOBS="${MLC_JOBS:-$(nproc 2>/dev/null || echo 4)}"

if [ -n "$MLC_CXX" ]; then
  CXX_CMD=($MLC_CXX)
elif command -v ccache &>/dev/null && command -v clang++ &>/dev/null; then
  CXX_CMD=(ccache clang++)
elif command -v clang++ &>/dev/null; then
  CXX_CMD=(clang++)
elif command -v ccache &>/dev/null; then
  CXX_CMD=(ccache g++)
else
  CXX_CMD=(g++)
fi

OBJ_DIR="$(mktemp -d)"
trap "rm -rf $OBJ_DIR" EXIT

INC_FLAGS=(-I "$CPP_DIR" -I "$RT_INC")
[ -n "$EXTRA_INC" ] && INC_FLAGS+=(-I "$EXTRA_INC")

ALL_CPP=("$CPP_DIR"/*.cpp "${RT_SRC[@]}")
OBJS=()
PIDS=()
ERRORS=()
SLOT=0

for cpp in "${ALL_CPP[@]}"; do
  [ -f "$cpp" ] || continue
  obj="$OBJ_DIR/$(basename "$cpp" .cpp)_$SLOT.o"
  OBJS+=("$obj")
  "${CXX_CMD[@]}" -std=c++20 -O2 "${INC_FLAGS[@]}" -c "$cpp" -o "$obj" &
  PIDS+=($!)
  SLOT=$((SLOT + 1))
  if [ ${#PIDS[@]} -ge "$JOBS" ]; then
    for pid in "${PIDS[@]}"; do
      wait "$pid" || ERRORS+=("$pid")
    done
    PIDS=()
  fi
done

for pid in "${PIDS[@]}"; do
  wait "$pid" || ERRORS+=("$pid")
done

if [ ${#ERRORS[@]} -gt 0 ]; then
  echo "build_bin.sh: compile errors" >&2
  exit 1
fi

LINK_FLAGS=()
command -v ld.lld &>/dev/null && LINK_FLAGS+=(-fuse-ld=lld)

"${CXX_CMD[@]}" -std=c++20 "${LINK_FLAGS[@]}" -o "$BIN_OUT" "${OBJS[@]}"
