#!/usr/bin/env bash
# TRACK_EDITOR_TEXT_DRAW_SCROLL_X STEP=1 — draw still ignores scroll_x.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_text_draw_scroll_x_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux text_draw_scroll_x_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: syntax row pen uses text_rect.x + 4 without subtracting scroll_offset_x.
if ! grep -q 'editor_ux_append_syntax_colored_row' "$DEMO"; then
  echo "[ux text_draw_scroll_x_red] FAIL: missing append_syntax_colored_row" >&2
  exit 1
fi
if ! grep -A12 'editor_ux_append_syntax_colored_row' "$DEMO" | grep -q 'text_rect.x + 4'; then
  echo "[ux text_draw_scroll_x_red] FAIL: syntax row missing text_rect.x + 4" >&2
  exit 1
fi
if grep -A12 'editor_ux_append_syntax_colored_row' "$DEMO" | grep -qE 'scroll_offset_x|text_draw_scroll'; then
  echo "[ux text_draw_scroll_x_red] FAIL: syntax row already uses scroll_x (expected gap)" >&2
  exit 1
fi

# Caret draw: text_rect.x + 4 + pixel without scroll subtract.
if ! grep -q 'const caret_x = text_rect.x + 4 + pixel_x_for_byte_offset' "$DEMO"; then
  echo "[ux text_draw_scroll_x_red] FAIL: caret missing text_rect.x + 4 + pixel" >&2
  exit 1
fi
if grep -B2 -A8 'const caret_x = text_rect.x + 4 + pixel_x_for_byte_offset' "$DEMO" | grep -qE 'scroll_offset_x| - app\.ux\.scroll'; then
  echo "[ux text_draw_scroll_x_red] FAIL: caret already subtracts scroll_x (expected gap)" >&2
  exit 1
fi

# Selection fill: text_rect.x + 4 + span without scroll.
if ! grep -q 'text_rect.x + 4 + span.x0' "$DEMO"; then
  echo "[ux text_draw_scroll_x_red] FAIL: selection fill missing text_rect.x + 4 + span" >&2
  exit 1
fi
if grep -B2 -A2 'text_rect.x + 4 + span.x0' "$DEMO" | grep -q 'scroll_offset_x'; then
  echo "[ux text_draw_scroll_x_red] FAIL: selection fill already uses scroll_x (expected gap)" >&2
  exit 1
fi

# No dedicated draw-scroll helper marker yet.
if grep -qE 'text_draw_scroll_x|text_pen_x_scrolled' "$DEMO"; then
  echo "[ux text_draw_scroll_x_red] FAIL: demo already has draw-scroll helper (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux text_draw_scroll_x_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok text_draw_scroll_x_stable|text draw scroll_x ok' "$STABLE"; then
  echo "[ux text_draw_scroll_x_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux text_draw_scroll_x_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok text_draw_scroll_x_red"
echo "[ux text_draw_scroll_x_red] ok" >&2
