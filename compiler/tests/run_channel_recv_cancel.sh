#!/usr/bin/env bash
# TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING — green once STEP=2 wires MLC StopToken + recv(token).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/channel_recv_with_stop_token.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[channel_recv_cancel] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if [ "$status" -ne 0 ]; then
  echo "[channel_recv_cancel] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -qE 'error\[E001\]|error\[E014\]'; then
  echo "[channel_recv_cancel] FAIL: still E001/E014 (expected green after STEP=2)" >&2
  exit 1
fi

echo "ok channel_recv_cancel"
echo "[channel_recv_cancel] ok" >&2
