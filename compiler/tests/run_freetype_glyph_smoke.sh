#!/usr/bin/env bash
# FreeType one-glyph smoke (TRACK_TEXT_RENDERING STEP=2).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${FREETYPE_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/freetype_glyph_smoke.mlc}"
OUT_DIR="${FREETYPE_SMOKE_OUT:-$ROOT_DIR/tmp/freetype_glyph_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists freetype2; then
  echo "[freetype glyph smoke] FAIL: freetype2 not found (pkg-config)" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[freetype glyph smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[freetype glyph smoke] FAIL missing: $ENTRY" >&2
  exit 1
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf ]; then
  echo "[freetype glyph smoke] FAIL: DejaVuSans.ttf not found" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
mkdir -p "$OUT_DIR"
rm -rf "$OUT_DIR"/*
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
"$BIN_OUT"
status=$?
if [ "$status" -ne 0 ]; then
  echo "[freetype glyph smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[freetype glyph smoke] ok" >&2
