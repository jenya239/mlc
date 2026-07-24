#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_SYNTAX_TAGS STEP=1 — minimap glyphs still uniform text color.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_minimap_syntax_tags_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_syntax_tags_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'editor_ux_minimap_cache_tick' "$DEMO"; then
  echo "[ux minimap_syntax_tags_red] FAIL: missing minimap rebuild" >&2
  exit 1
fi

# Gap: rebuild uses bare push_line; draw uses uniform theme.text_*.
if ! grep -A30 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'push_line('; then
  echo "[ux minimap_syntax_tags_red] FAIL: missing bare push_line in minimap rebuild" >&2
  exit 1
fi
if grep -A30 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'editor_ux_append_syntax_colored_row'; then
  echo "[ux minimap_syntax_tags_red] FAIL: minimap already uses syntax rows (expected gap)" >&2
  exit 1
fi
if ! grep -A40 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'theme.text_red'; then
  echo "[ux minimap_syntax_tags_red] FAIL: missing uniform theme.text_ draw" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux minimap_syntax_tags_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok minimap_syntax_tags_stable' "$STABLE"; then
  echo "[ux minimap_syntax_tags_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux minimap_syntax_tags_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok minimap_syntax_tags_red"
echo "[ux minimap_syntax_tags_red] ok" >&2
