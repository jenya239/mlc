#!/usr/bin/env bash
# TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK STEP=1 —
# Confirm TaskScope.spawn free let mut capture is still missing E087 (gap red).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/task_scope_spawn_mutable_capture.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[task_scope_spawn_mutable_capture_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if printf '%s\n' "$output" | grep -q 'error\[E087\]'; then
  echo "[task_scope_spawn_mutable_capture_red] FAIL: E087 already present (expected red gap)" >&2
  exit 1
fi

echo "ok task_scope_spawn_mutable_capture_red"
echo "[task_scope_spawn_mutable_capture_red] ok (no E087 yet, status=$status)" >&2
