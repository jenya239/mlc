#!/usr/bin/env bash
# TRACK_CONCURRENCY_MOVE_TRACKING STEP=1 —
# Confirm TaskScope.spawn move then use still missing E088 (gap red).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/task_scope_spawn_move_then_use.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[task_scope_spawn_move_then_use_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if printf '%s\n' "$output" | grep -q 'error\[E088\]'; then
  echo "[task_scope_spawn_move_then_use_red] FAIL: E088 already present (expected red gap)" >&2
  exit 1
fi

echo "ok task_scope_spawn_move_then_use_red"
echo "[task_scope_spawn_move_then_use_red] ok (no E088 yet, status=$status)" >&2
