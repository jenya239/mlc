#!/usr/bin/env bash
# IM Button + easing demo (TRACK_GUI_FRAMEWORK STEP=5).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GUI_BUTTON_DEMO_ENTRY:-$ROOT_DIR/misc/examples/gui_button_demo.mlc}"
OUT_DIR="${GUI_BUTTON_DEMO_OUT:-$ROOT_DIR/tmp/gui_button_demo}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[gui button demo] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[gui button demo] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[gui button demo] FAIL missing: $ENTRY" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[gui button demo] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[gui button demo] ok" >&2
