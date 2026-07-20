#!/usr/bin/env bash
# TRACK_CONCURRENCY_SYNC_TRAIT STEP=1 —
# Confirm move Array[i32] into spawn is still wrongly rejected with E092 (Send gap).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/spawn_move_array_send.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[spawn_move_array_send_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E092\]'; then
  echo "[spawn_move_array_send_red] FAIL: missing E092 (expected red Send gap on move Array)" >&2
  exit 1
fi

echo "ok spawn_move_array_send_red"
echo "[spawn_move_array_send_red] ok (E092 still present, status=$status)" >&2
