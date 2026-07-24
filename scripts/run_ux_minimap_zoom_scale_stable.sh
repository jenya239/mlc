#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_ZOOM_SCALE — L0: minimap pixel size scales with font zoom.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_zoom_scale_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$MINIMAP" ]; then
  echo "[ux minimap_zoom_scale_stable] FAIL: missing $MINIMAP" >&2
  exit 1
fi

if ! grep -q 'export fn editor_ux_minimap_pixel_size_for_font' "$MINIMAP"; then
  echo "[ux minimap_zoom_scale_stable] FAIL: missing for_font helper" >&2
  exit 1
fi

if ! grep -A45 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'editor_ux_minimap_pixel_size_for_font(zoom_font_size)'; then
  echo "[ux minimap_zoom_scale_stable] FAIL: demo rebuild missing for_font(zoom_font_size)" >&2
  exit 1
fi

if ! grep -q 'minimap_cached_font_size' "$DEMO"; then
  echo "[ux minimap_zoom_scale_stable] FAIL: missing zoom rebuild sentinel" >&2
  exit 1
fi

echo "ux_ok minimap_zoom_scale_stable"
echo "[ux minimap_zoom_scale_stable] ok" >&2
