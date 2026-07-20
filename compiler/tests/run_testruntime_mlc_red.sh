#!/usr/bin/env bash
# TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE STEP=1 —
# Confirm TestRuntime MLC surface missing (E001) while C++ scheduler.hpp exists.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/testruntime_new_spawn.mlc"
SCHEDULER_HPP="$ROOT_DIR/runtime/include/mlc/concurrency/testing/scheduler.hpp"

if [ ! -x "$MLCC" ]; then
  echo "[testruntime_mlc_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if [ ! -f "$SCHEDULER_HPP" ]; then
  echo "[testruntime_mlc_red] FAIL: scheduler.hpp missing (expected C++ already present)" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[testruntime_mlc_red] FAIL: missing E001 (expected undefined TestRuntime)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'undefined: TestRuntime'; then
  echo "[testruntime_mlc_red] FAIL: missing 'undefined: TestRuntime'" >&2
  exit 1
fi
if [ "$status" -eq 0 ]; then
  echo "[testruntime_mlc_red] FAIL: check-only succeeded (expected red gap)" >&2
  exit 1
fi

echo "ok testruntime_mlc_red"
echo "[testruntime_mlc_red] ok (E001 TestRuntime, status=$status, scheduler.hpp present)" >&2
