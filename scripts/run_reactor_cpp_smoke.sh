#!/usr/bin/env bash
# Reactor timers and block_on. Links libcurl because EventLoop owns CURLM*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SOURCE="$ROOT_DIR/runtime/test/test_reactor_timers.cpp"
BLOCK_SOURCE="$ROOT_DIR/runtime/test/test_reactor_block_on.cpp"
DIRECTORY="${REACTOR_CPP_DIR:-${TMPDIR:-$ROOT_DIR/tmp}/reactor_cpp}"
CXX="${CXX:-c++}"

mkdir -p "$DIRECTORY"

if ! pkg-config --exists libcurl; then
  echo "[reactor] FAIL: libcurl pkg-config missing" >&2
  exit 1
fi
LINK_FLAGS=($(pkg-config --libs libcurl))

echo "[reactor] stage=compile" >&2
if ! "$CXX" -std=c++20 -pthread -Wall -Wextra -I"$ROOT_DIR/runtime/include" \
  -o "$DIRECTORY/test_reactor_timers" "$SOURCE" "${LINK_FLAGS[@]}"; then
  echo "[reactor] FAIL stage=compile" >&2
  exit 1
fi

echo "[reactor] stage=run" >&2
if ! "$DIRECTORY/test_reactor_timers"; then
  echo "[reactor] FAIL stage=run" >&2
  exit 1
fi

echo "[reactor] stage=compile_block_on" >&2
if ! "$CXX" -std=c++20 -pthread -Wall -Wextra -I"$ROOT_DIR/runtime/include" \
  -o "$DIRECTORY/test_reactor_block_on" "$BLOCK_SOURCE" "${LINK_FLAGS[@]}"; then
  echo "[reactor] FAIL stage=compile_block_on" >&2
  exit 1
fi

echo "[reactor] stage=run_block_on" >&2
if ! "$DIRECTORY/test_reactor_block_on"; then
  echo "[reactor] FAIL stage=run_block_on" >&2
  exit 1
fi

echo "[reactor] ok"
