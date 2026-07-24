#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_WIDTH_ZOOM — L0: strip width scales with font zoom.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_width_zoom_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$MINIMAP" ]; then
  echo "[ux minimap_width_zoom_stable] FAIL: missing $MINIMAP" >&2
  exit 1
fi

if ! grep -q 'export fn editor_ux_minimap_width_for_font' "$MINIMAP"; then
  echo "[ux minimap_width_zoom_stable] FAIL: missing width_for_font helper" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_minimap_layout_for_font' "$MINIMAP"; then
  echo "[ux minimap_width_zoom_stable] FAIL: missing layout_for_font helper" >&2
  exit 1
fi

if ! grep -q 'editor_ux_minimap_layout_for_font(content_band, zoom_font_size)' "$DEMO"; then
  echo "[ux minimap_width_zoom_stable] FAIL: demo missing layout_for_font(zoom)" >&2
  exit 1
fi

echo "ux_ok minimap_width_zoom_stable"
echo "[ux minimap_width_zoom_stable] ok" >&2
