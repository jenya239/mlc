#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_GLYPH_X §111a1 S3 Red — adjust_x / scenario absent.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
GREEN="$ROOT_DIR/scripts/run_editor_horizontal_scroll_glyphs.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/horizontal_scroll_moves_glyphs.mlc"

fail() {
  echo "[editor horizontal scroll glyphs red] FAIL: $*" >&2
  exit 1
}

if [ -f "$GREEN" ] && [ -f "$SCENARIO" ] &&
    grep -q 'static_text_glyph_batch_adjust_x' "$STATIC_TEXT" &&
    grep -q 'static_text_glyph_batch_adjust_x' "$DEMO" &&
    grep -q 'editor_glyph_scroll_x' "$DEMO"; then
  echo "[editor horizontal scroll glyphs red] FAIL: already present" >&2
  exit 1
fi

fail "no horizontal scroll glyph adjust_x wire (Red expected)"
