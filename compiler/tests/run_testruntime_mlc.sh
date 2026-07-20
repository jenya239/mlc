#!/usr/bin/env bash
# TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE — green once STEP=2 wires TestRuntime.new/spawn/join.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/testruntime_new_spawn.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[testruntime_mlc] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if [ "$status" -ne 0 ]; then
  echo "[testruntime_mlc] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[testruntime_mlc] FAIL: still E001 (expected green after STEP=2)" >&2
  exit 1
fi

echo "ok testruntime_mlc"
echo "[testruntime_mlc] ok" >&2
