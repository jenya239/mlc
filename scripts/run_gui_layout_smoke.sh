#!/usr/bin/env bash
# IM layout + point_in_rect (TRACK_GUI_FRAMEWORK STEP=3). No GLFW required.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GUI_LAYOUT_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/gui_layout_smoke.mlc}"
OUT_DIR="${GUI_LAYOUT_SMOKE_OUT:-$ROOT_DIR/tmp/gui_layout_smoke}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[gui layout smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[gui layout smoke] FAIL missing: $ENTRY" >&2
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
  echo "[gui layout smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[gui layout smoke] ok" >&2
