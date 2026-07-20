#!/usr/bin/env bash
# TRACK_CONCURRENCY_FFI_METADATA STEP=1 —
# Confirm thread_affine extern fn in spawn / TaskScope.spawn has no E094 yet.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
SPAWN_FIXTURE="$COMPILER_DIR/tests/fixtures/extern_thread_affine_spawn.mlc"
SCOPE_FIXTURE="$COMPILER_DIR/tests/fixtures/extern_thread_affine_task_scope_spawn.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[extern_thread_affine_spawn_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

check_red() {
  local label="$1"
  local fixture="$2"
  set +e
  local output
  local status
  output=$("$MLCC" --check-only "$fixture" 2>&1)
  status=$?
  set -e
  printf '%s\n' "$output"
  if printf '%s\n' "$output" | grep -q 'error\[E094\]'; then
    echo "[extern_thread_affine_spawn_red] FAIL: $label already has E094 (expected red gap)" >&2
    exit 1
  fi
  if [ "$status" -ne 0 ]; then
    echo "[extern_thread_affine_spawn_red] FAIL: $label check-only status=$status (expected 0 while gap open)" >&2
    exit 1
  fi
}

check_red 'bare spawn' "$SPAWN_FIXTURE"
check_red 'TaskScope.spawn' "$SCOPE_FIXTURE"

echo "ok extern_thread_affine_spawn_red"
echo "[extern_thread_affine_spawn_red] ok (no E094 yet, both EXIT=0)" >&2
