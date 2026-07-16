#!/usr/bin/env bash
# TRACK_SCENE_FORM_LIVE — interactive (Esc quit); CI uses run_scene_form_live_compile.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/scene_form_live.mlc"
OUT_DIR="${SCENE_FORM_LIVE_OUT:-$ROOT_DIR/tmp/scene_form_live}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[scene form live] FAIL: glfw3 not found" >&2
  exit 1
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[scene form live] FAIL: DejaVuSansMono.ttf not found" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[scene form live] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE=1

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
exec "$BIN_OUT"
