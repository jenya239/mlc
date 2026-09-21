#!/usr/bin/env bash
# PLAN §112 / Cairo 01-label: visible three-Label gallery via app.mlc.
# Default: show the GLFW window (playground contract). Headless:
#   MLC_GLFW_VISIBLE=0 MLC_TEXTUI_BENCH=1 bash scripts/run_textui_label_gallery.sh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXTUI_GALLERY_ENTRY:-$ROOT_DIR/misc/textui/app.mlc}"
OUT_DIR="${TEXTUI_GALLERY_OUT:-$ROOT_DIR/tmp/textui_label_gallery}"
BIN_OUT="$OUT_DIR/bin"
FONT="${TEXTUI_GALLERY_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[textui gallery] SKIP: glfw3 not found" >&2
  exit 0
fi
if ! pkg-config --exists freetype2 || ! pkg-config --exists harfbuzz; then
  echo "[textui gallery] SKIP: freetype2/harfbuzz not found" >&2
  exit 0
fi
if [ ! -f "$FONT" ]; then
  echo "[textui gallery] SKIP: font missing $FONT" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[textui gallery] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-1}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
if [ "${MLC_TEXTUI_COMPILE_ONLY:-0}" = "1" ]; then
  echo "[textui gallery] compiled $BIN_OUT" >&2
  exit 0
fi
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -eq 75 ]; then
  exec ruby "$ROOT_DIR/scripts/launch_textui_gallery.rb"
fi
if [ "$status" -ne 0 ]; then
  echo "[textui gallery] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui gallery] ok MLC_GLFW_VISIBLE=$MLC_GLFW_VISIBLE" >&2
