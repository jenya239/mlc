#!/usr/bin/env bash
# TRACK_CONCURRENCY_SYNC_TRAIT — green once STEP=2 retunes Array Send + E093 free !Sync.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE_MOVE="$COMPILER_DIR/tests/fixtures/spawn_move_array_send.mlc"
FIXTURE_FREE="$COMPILER_DIR/tests/fixtures/spawn_free_array_not_sync.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[spawn_array_sync] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
move_output=$("$MLCC" --check-only "$FIXTURE_MOVE" 2>&1)
move_status=$?
free_output=$("$MLCC" --check-only "$FIXTURE_FREE" 2>&1)
free_status=$?
set -e
printf '%s\n' "$move_output"
printf '%s\n' "$free_output"

if printf '%s\n' "$move_output" | grep -qE 'error\[E092\]|error\[E093\]'; then
  echo "[spawn_array_sync] FAIL: move Array still has E092/E093 (expected green after Send retune)" >&2
  exit 1
fi
if [ "$move_status" -ne 0 ]; then
  echo "[spawn_array_sync] FAIL: move Array check-only status=$move_status (expected 0)" >&2
  exit 1
fi
if ! printf '%s\n' "$free_output" | grep -q 'error\[E093\]'; then
  echo "[spawn_array_sync] FAIL: missing error[E093] on free Array (expected red until STEP=2)" >&2
  exit 1
fi

echo "ok spawn_array_sync_e093"
echo "[spawn_array_sync] ok" >&2
