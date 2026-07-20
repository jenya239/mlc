#!/usr/bin/env bash
# TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING STEP=2 —
# Compile+run: pre-requested token → Channel.recv returns Cancelled (exit 0).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/channel_recv_cancel_smoke.mlc"
OUT_DIR="${TMPDIR:-/tmp}/mlc_channel_recv_cancel_smoke_$$"
BIN_OUT="$OUT_DIR/channel_recv_cancel_smoke"

if [ ! -x "$MLCC" ]; then
  echo "[channel_recv_cancel_smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

mkdir -p "$OUT_DIR"
cleanup() { rm -rf "$OUT_DIR"; }
trap cleanup EXIT

"$MLCC" -o "$OUT_DIR" "$FIXTURE"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"

set +e
"$BIN_OUT"
status=$?
set -e

if [ "$status" -ne 0 ]; then
  echo "[channel_recv_cancel_smoke] FAIL: exit=$status (expected 0 Cancelled)" >&2
  exit 1
fi

echo "ok channel_recv_cancel_smoke"
echo "[channel_recv_cancel_smoke] ok" >&2
