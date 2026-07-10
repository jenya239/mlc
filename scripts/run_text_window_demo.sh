#!/usr/bin/env bash
# text_window_demo (TRACK_TEXT_RENDERING_NATIVE STEP=6).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXT_WINDOW_DEMO_ENTRY:-$ROOT_DIR/misc/examples/text_window_demo.mlc}"
OUT_DIR="${TEXT_WINDOW_DEMO_OUT:-$ROOT_DIR/tmp/text_window_demo}"
BIN_OUT="$OUT_DIR/bin"
FONT="${TEXT_WINDOW_DEMO_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[text window demo] SKIP: glfw3 not found" >&2
  exit 0
fi
if ! pkg-config --exists freetype2 || ! pkg-config --exists harfbuzz; then
  echo "[text window demo] SKIP: freetype2/harfbuzz not found" >&2
  exit 0
fi
if [ ! -f "$FONT" ]; then
  echo "[text window demo] SKIP: font missing $FONT" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[text window demo] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[text window demo] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[text window demo] ok" >&2
