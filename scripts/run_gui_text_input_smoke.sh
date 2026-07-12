#!/usr/bin/env bash
# TRACK_GUI_INPUT_ROBUSTNESS STEP=3 — GuiInput.text_buffer / keys inject.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GUI_TEXT_INPUT_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/gui_text_input_smoke.mlc}"
OUT_DIR="${GUI_TEXT_INPUT_SMOKE_OUT:-$ROOT_DIR/tmp/gui_text_input_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[gui text input smoke] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [[ ! -x "$MLCC" ]]; then
  echo "[gui text input smoke] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[gui text input smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[gui text input smoke] ok (take_text + keys inject)" >&2
