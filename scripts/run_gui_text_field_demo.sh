#!/usr/bin/env bash
# TRACK_GUI_INPUT_ROBUSTNESS STEP=4 — text field + TextRenderer inject smoke.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GUI_TEXT_FIELD_DEMO_ENTRY:-$ROOT_DIR/misc/examples/gui_text_field_demo.mlc}"
OUT_DIR="${GUI_TEXT_FIELD_DEMO_OUT:-$ROOT_DIR/tmp/gui_text_field_demo}"
BIN_OUT="$OUT_DIR/bin"
FONT="${GUI_TEXT_FIELD_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[gui text field demo] SKIP: glfw3 not found" >&2
  exit 0
fi
if ! pkg-config --exists freetype2 || ! pkg-config --exists harfbuzz; then
  echo "[gui text field demo] SKIP: freetype2/harfbuzz not found" >&2
  exit 0
fi
if [[ ! -f "$FONT" ]]; then
  echo "[gui text field demo] SKIP: font missing $FONT" >&2
  exit 0
fi
if [[ ! -x "$MLCC" ]]; then
  echo "[gui text field demo] FAIL: mlcc not found at $MLCC" >&2
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
if [[ "$status" -ne 0 ]]; then
  echo "[gui text field demo] FAIL binary exit=$status (want field==Hi + enter)" >&2
  exit 1
fi
echo "[gui text field demo] ok (field=Hi, enter edge)" >&2
