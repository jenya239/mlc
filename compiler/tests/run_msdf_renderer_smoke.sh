#!/usr/bin/env bash
# MSDF median shader FBO + RGB8 atlas pack (TRACK_TEXT_RENDERING STEP=7.2).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${MSDF_RENDERER_ENTRY:-$ROOT_DIR/misc/examples/msdf_renderer_smoke.mlc}"
OUT_DIR="${MSDF_RENDERER_OUT:-$ROOT_DIR/tmp/msdf_renderer_smoke}"
BIN_OUT="$OUT_DIR/bin"

need_pkg() {
  if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists "$1"; then
    echo "[msdf renderer] FAIL: pkg-config $1 missing" >&2
    exit 1
  fi
}
need_pkg egl
need_pkg glesv2
need_pkg freetype2

FONT="${MSDF_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"
if [ ! -f "$FONT" ]; then
  echo "[msdf renderer] FAIL missing font: $FONT" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[msdf renderer] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[msdf renderer] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[msdf renderer] ok" >&2
