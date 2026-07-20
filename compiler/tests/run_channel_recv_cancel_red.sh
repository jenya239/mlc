#!/usr/bin/env bash
# TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING STEP=1 —
# Confirm MLC cannot express cancel-wake: StopSource missing and/or recv(token) arity.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/channel_recv_with_stop_token.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[channel_recv_cancel_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

has_stop_gap=0
has_recv_gap=0
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  has_stop_gap=1
fi
if printf '%s\n' "$output" | grep -q 'error\[E014\]'; then
  has_recv_gap=1
fi

if [ "$has_stop_gap" -eq 0 ] && [ "$has_recv_gap" -eq 0 ]; then
  echo "[channel_recv_cancel_red] FAIL: expected E001 (StopSource) and/or E014 (recv arity)" >&2
  exit 1
fi
if [ "$status" -eq 0 ]; then
  echo "[channel_recv_cancel_red] FAIL: check-only succeeded (expected red gap)" >&2
  exit 1
fi

echo "ok channel_recv_cancel_red"
echo "[channel_recv_cancel_red] ok (StopSource/recv gaps present, status=$status)" >&2
