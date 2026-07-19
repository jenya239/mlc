#!/usr/bin/env bash
# TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX — scenario selection_highlight_matches_glyph_x (L1/L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/selection_highlight_matches_glyph_x.mlc"
OUT_DIR="${UX_SELECTION_HIGHLIGHT_MATCHES_GLYPH_X_OUT:-$ROOT_DIR/tmp/ux_selection_highlight_matches_glyph_x}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux selection_highlight_matches_glyph_x] FAIL: mlcc not found at $MLCC" >&2
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
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux selection_highlight_matches_glyph_x] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok selection_highlight_matches_glyph_x'; then
  echo "[ux selection_highlight_matches_glyph_x] FAIL: missing ux_ok selection_highlight_matches_glyph_x" >&2
  exit 1
fi
echo "[ux selection_highlight_matches_glyph_x] ok" >&2
