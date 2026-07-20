#!/usr/bin/env bash
# TRACK_CONCURRENCY_SEND_BOUND — green gate once STEP=2 wires E092 on spawn sites.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE_BARE="$COMPILER_DIR/tests/fixtures/spawn_shared_capture_not_send.mlc"
FIXTURE_SCOPE="$COMPILER_DIR/tests/fixtures/task_scope_spawn_shared_capture_not_send.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[spawn_shared_capture_not_send] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

check_e092() {
  local fixture="$1"
  local label="$2"
  set +e
  local output
  output=$("$MLCC" --check-only "$fixture" 2>&1)
  set -e
  printf '%s\n' "$output"
  if ! printf '%s\n' "$output" | grep -q 'error\[E092\]'; then
    echo "[spawn_shared_capture_not_send] FAIL: missing error[E092] on $label (expected red until STEP=2)" >&2
    exit 1
  fi
  echo "[spawn_shared_capture_not_send] $label ok" >&2
}

check_e092 "$FIXTURE_BARE" "bare spawn"
check_e092 "$FIXTURE_SCOPE" "TaskScope.spawn"

echo "ok spawn_shared_capture_not_send_e092"
echo "[spawn_shared_capture_not_send] ok" >&2
