#!/usr/bin/env bash
# TRACK_CONCURRENCY_ATOMICS — green once STEP=2 wires AtomicI32.new / fetch_add.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/atomic_i32_fetch_add.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[atomic_i32] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if [ "$status" -ne 0 ]; then
  echo "[atomic_i32] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[atomic_i32] FAIL: still E001 (expected green after STEP=2)" >&2
  exit 1
fi

echo "ok atomic_i32"
echo "[atomic_i32] ok" >&2
