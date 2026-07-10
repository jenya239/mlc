#!/usr/bin/env bash
# RenderMode A8/MSDF switch by pixel size (TRACK_TEXT_RENDERING STEP=7.3).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${RENDER_MODE_ENTRY:-$ROOT_DIR/misc/examples/render_mode_smoke.mlc}"
OUT_DIR="${RENDER_MODE_OUT:-$ROOT_DIR/tmp/render_mode_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists freetype2; then
  echo "[render mode] FAIL: freetype2 missing" >&2
  exit 1
fi
FONT="${RENDER_MODE_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"
if [ ! -f "$FONT" ]; then
  echo "[render mode] FAIL missing font: $FONT" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[render mode] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[render mode] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[render mode] ok" >&2
