#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_GLYPH_X §111a1 S3 Green — adjust_x on retained glyphs.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
ABI="$ROOT_DIR/runtime/include/mlc/gl/glad_gl_abi.hpp"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/horizontal_scroll_moves_glyphs.mlc"
OUT_DIR="${EDITOR_HORIZONTAL_SCROLL_GLYPHS_OUT:-$ROOT_DIR/tmp/editor_horizontal_scroll_glyphs}"
BIN_OUT="$OUT_DIR/bin"

fail() {
  echo "[editor horizontal scroll glyphs] FAIL: $*" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found"
[ -f "$ENTRY" ] || fail "missing scenario"
[ -f "$DEMO" ] || fail "missing demo_live"
[ -f "$STATIC_TEXT" ] || fail "missing static_text"

grep -q 'static_text_glyph_batch_adjust_x' "$STATIC_TEXT" || fail "missing adjust_x"
grep -q 'buffer_adjust_ndc_x' "$ABI" || fail "missing buffer_adjust_ndc_x"
grep -q 'static_text_glyph_batch_adjust_x' "$DEMO" || fail "demo_live missing adjust_x call"
grep -q 'editor_glyph_scroll_x' "$DEMO" || fail "demo_live missing editor_glyph_scroll_x"
# Sabotage: fp must not force reshape on scroll_x (same as Y).
if awk '
  /editor_glyph_fp =$/ || /editor_glyph_fp =/ { in_fp=1 }
  in_fp {
    if ($0 ~ /scroll_offset_x/) found=1
    if ($0 ~ /theme\.keyword_red/ || $0 ~ /editor_lines\.length/) in_fp=0
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live editor_glyph_fp includes scroll_offset_x (Decision sabotage)"
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
[ "$status" -eq 0 ] || fail "binary exit=$status"
printf '%s\n' "$output" | grep -q 'ux_ok horizontal_scroll_moves_glyphs' ||
  fail "missing ux_ok"

echo "[editor horizontal scroll glyphs] ok" >&2
