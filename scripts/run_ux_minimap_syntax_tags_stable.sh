#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_SYNTAX_TAGS — L0: minimap rebuild uses syntax-colored rows.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_syntax_tags_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -A40 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'editor_ux_append_syntax_colored_row'; then
  echo "[ux minimap_syntax_tags_stable] FAIL: minimap missing append_syntax_colored_row" >&2
  exit 1
fi

if ! grep -A40 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'highlight_mlc'; then
  echo "[ux minimap_syntax_tags_stable] FAIL: minimap missing full-buffer highlight_mlc" >&2
  exit 1
fi

# Bare push_line for map text must be gone from rebuild.
if grep -A40 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'push_line('; then
  echo "[ux minimap_syntax_tags_stable] FAIL: minimap rebuild still uses bare push_line" >&2
  exit 1
fi

echo "ux_ok minimap_syntax_tags_stable"
echo "[ux minimap_syntax_tags_stable] ok" >&2
