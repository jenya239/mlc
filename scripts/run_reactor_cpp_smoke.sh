#!/usr/bin/env bash
# Reactor step 1: two timers on one thread. No libcurl.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SOURCE="$ROOT_DIR/runtime/test/test_reactor_timers.cpp"
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

echo "[reactor] ok"
