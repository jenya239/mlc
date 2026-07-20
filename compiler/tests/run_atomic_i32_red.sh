#!/usr/bin/env bash
# TRACK_CONCURRENCY_ATOMICS STEP=1 —
# Confirm AtomicI32 is undefined (E001) and atomic.hpp missing.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/atomic_i32_fetch_add.mlc"
ATOMIC_HPP="$ROOT_DIR/runtime/include/mlc/concurrency/atomic.hpp"

if [ ! -x "$MLCC" ]; then
  echo "[atomic_i32_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if [ -f "$ATOMIC_HPP" ]; then
  echo "[atomic_i32_red] FAIL: atomic.hpp already exists (expected red gap)" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[atomic_i32_red] FAIL: missing E001 (expected undefined AtomicI32)" >&2
  exit 1
fi
if [ "$status" -eq 0 ]; then
  echo "[atomic_i32_red] FAIL: check-only succeeded (expected red gap)" >&2
  exit 1
fi

echo "ok atomic_i32_red"
echo "[atomic_i32_red] ok (E001 present, status=$status)" >&2
