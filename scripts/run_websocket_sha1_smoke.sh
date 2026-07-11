#!/usr/bin/env bash
# TRACK_STDLIB_WEBSOCKET_TO_MLC STEP=2 — MLC SHA1/base64 RFC vector.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_websocket_mlc}/websocket_sha1_smoke"

if [[ ! -x "$MLCC" ]]; then
  echo "[websocket sha1] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[websocket sha1] codegen" >&2
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/examples/websocket_sha1_smoke.mlc"

echo "[websocket sha1] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=websocket_sha1_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/websocket_sha1_smoke"

set +e
"$OUT_DIR/websocket_sha1_smoke"
status=$?
set -e
if [[ "$status" -ne 0 ]]; then
  echo "[websocket sha1] FAIL exit=$status expected=0" >&2
  exit 1
fi
echo "[websocket sha1] ok exit=$status" >&2
