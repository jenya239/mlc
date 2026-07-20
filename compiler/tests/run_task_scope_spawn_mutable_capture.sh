#!/usr/bin/env bash
# TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK — green gate once STEP=2 wires E087 on TaskScope.spawn.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/task_scope_spawn_mutable_capture.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[task_scope_spawn_mutable_capture] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E087\]'; then
  echo "[task_scope_spawn_mutable_capture] FAIL: missing error[E087] (expected red until STEP=2)" >&2
  exit 1
fi

echo "ok task_scope_spawn_mutable_capture_e087"
echo "[task_scope_spawn_mutable_capture] ok" >&2
