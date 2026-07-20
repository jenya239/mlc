#!/usr/bin/env bash
# TRACK_CONCURRENCY_SEND_BOUND STEP=1 —
# Confirm spawn / TaskScope.spawn Shared capture still missing E092 (gap red).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE_BARE="$COMPILER_DIR/tests/fixtures/spawn_shared_capture_not_send.mlc"
FIXTURE_SCOPE="$COMPILER_DIR/tests/fixtures/task_scope_spawn_shared_capture_not_send.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[spawn_shared_capture_not_send_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

check_gap() {
  local fixture="$1"
  local label="$2"
  set +e
  local output
  output=$("$MLCC" --check-only "$fixture" 2>&1)
  local status=$?
  set -e
  printf '%s\n' "$output"
  if printf '%s\n' "$output" | grep -q 'error\[E092\]'; then
    echo "[spawn_shared_capture_not_send_red] FAIL: E092 already present on $label (expected red gap)" >&2
    exit 1
  fi
  echo "[spawn_shared_capture_not_send_red] $label gap ok (no E092, status=$status)" >&2
}

check_gap "$FIXTURE_BARE" "bare spawn"
check_gap "$FIXTURE_SCOPE" "TaskScope.spawn"

echo "ok spawn_shared_capture_not_send_red"
echo "[spawn_shared_capture_not_send_red] ok" >&2
