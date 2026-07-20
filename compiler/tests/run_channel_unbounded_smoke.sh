#!/usr/bin/env bash
# TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED STEP=2 —
# Compile+run: unbounded accepts 256 sends then drains (exit 0).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/channel_unbounded_smoke.mlc"
OUT_DIR="${TMPDIR:-/tmp}/mlc_channel_unbounded_smoke_$$"
BIN_OUT="$OUT_DIR/channel_unbounded_smoke"

if [ ! -x "$MLCC" ]; then
  echo "[channel_unbounded_smoke] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[channel_unbounded_smoke] FAIL: exit=$status (expected 0)" >&2
  exit 1
fi

echo "ok channel_unbounded_smoke"
echo "[channel_unbounded_smoke] ok" >&2
