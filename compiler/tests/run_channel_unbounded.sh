#!/usr/bin/env bash
# TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED — green once STEP=2 wires make_unbounded_channel.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/channel_unbounded.mlc"
RENDEZVOUS="$COMPILER_DIR/tests/fixtures/channel_rendezvous_zero.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[channel_unbounded] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
rendezvous_output=$("$MLCC" --check-only "$RENDEZVOUS" 2>&1)
rendezvous_status=$?
set -e
printf '%s\n' "$output"
printf '%s\n' "$rendezvous_output"

if [ "$status" -ne 0 ]; then
  echo "[channel_unbounded] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[channel_unbounded] FAIL: still E001 (expected green after STEP=2)" >&2
  exit 1
fi
if [ "$rendezvous_status" -ne 0 ]; then
  echo "[channel_unbounded] FAIL: make_channel(0) check-only status=$rendezvous_status" >&2
  exit 1
fi

echo "ok channel_unbounded"
echo "[channel_unbounded] ok" >&2
