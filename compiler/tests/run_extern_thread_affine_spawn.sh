#!/usr/bin/env bash
# TRACK_CONCURRENCY_FFI_METADATA — green once STEP=2 wires E094 on affine fn in spawn.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
SPAWN_FIXTURE="$COMPILER_DIR/tests/fixtures/extern_thread_affine_spawn.mlc"
SCOPE_FIXTURE="$COMPILER_DIR/tests/fixtures/extern_thread_affine_task_scope_spawn.mlc"
MAIN_OK="$COMPILER_DIR/tests/fixtures/extern_thread_affine_main_ok.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[extern_thread_affine_spawn] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

check_green_error() {
  local label="$1"
  local fixture="$2"
  set +e
  local output
  local status
  output=$("$MLCC" --check-only "$fixture" 2>&1)
  status=$?
  set -e
  printf '%s\n' "$output"
  if ! printf '%s\n' "$output" | grep -q 'error\[E094\]'; then
    echo "[extern_thread_affine_spawn] FAIL: $label missing E094 (expected after STEP=2)" >&2
    exit 1
  fi
  if [ "$status" -eq 0 ]; then
    echo "[extern_thread_affine_spawn] FAIL: $label check-only succeeded (expected error after STEP=2)" >&2
    exit 1
  fi
}

check_green_error 'bare spawn' "$SPAWN_FIXTURE"
check_green_error 'TaskScope.spawn' "$SCOPE_FIXTURE"

if [ -f "$MAIN_OK" ]; then
  set +e
  main_output=$("$MLCC" --check-only "$MAIN_OK" 2>&1)
  main_status=$?
  set -e
  printf '%s\n' "$main_output"
  if [ "$main_status" -ne 0 ]; then
    echo "[extern_thread_affine_spawn] FAIL: main-thread call should stay OK" >&2
    exit 1
  fi
fi

echo "ok extern_thread_affine_spawn"
echo "[extern_thread_affine_spawn] ok" >&2
