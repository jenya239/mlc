#!/usr/bin/env bash
# Path segments, prefixes, and back/forward history. No GLFW.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXTUI_SLICE22_ENTRY:-$ROOT_DIR/misc/textui/test/slice22_path_bar.mlc}"
OUT_DIR="${TEXTUI_SLICE22_OUT:-$ROOT_DIR/tmp/textui_slice22_smoke}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[textui slice22] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[textui slice22] FAIL missing: $ENTRY" >&2
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
  echo "[textui slice22] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui slice22] ok" >&2
