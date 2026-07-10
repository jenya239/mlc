#!/usr/bin/env bash
# GlyphCache LRU smoke (TRACK_TEXT_RENDERING STEP=4.2). Pure MLC.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GLYPH_CACHE_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/glyph_cache_smoke.mlc}"
OUT_DIR="${GLYPH_CACHE_SMOKE_OUT:-$ROOT_DIR/tmp/glyph_cache_smoke}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[glyph cache smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[glyph cache smoke] FAIL missing: $ENTRY" >&2
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
  echo "[glyph cache smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[glyph cache smoke] ok" >&2
