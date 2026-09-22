#!/usr/bin/env bash
# TRACK_TEXTUI_FILE_MANAGER — frame request: layout vs paint vs idle.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXTUI_SLICE03_ENTRY:-$ROOT_DIR/misc/textui/test/slice03_frame.mlc}"
OUT_DIR="${TEXTUI_SLICE03_OUT:-$ROOT_DIR/tmp/textui_slice03_smoke}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[textui slice03] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[textui slice03] FAIL missing: $ENTRY" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[textui slice03] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui slice03] ok" >&2
