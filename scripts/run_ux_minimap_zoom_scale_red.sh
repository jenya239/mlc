#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_ZOOM_SCALE STEP=1 — minimap pixel size still fixed constant.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_minimap_zoom_scale_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_zoom_scale_red] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$MINIMAP" ]; then
  echo "[ux minimap_zoom_scale_red] FAIL: missing $MINIMAP" >&2
  exit 1
fi

# Gap: constant pixel_size helper only; no for_font / zoom wire.
if ! grep -q 'export fn editor_ux_minimap_pixel_size()' "$MINIMAP"; then
  echo "[ux minimap_zoom_scale_red] FAIL: missing constant pixel_size helper" >&2
  exit 1
fi
if grep -q 'editor_ux_minimap_pixel_size_for_font' "$MINIMAP"; then
  echo "[ux minimap_zoom_scale_red] FAIL: for_font helper already present (expected gap)" >&2
  exit 1
fi
if ! grep -A40 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'editor_ux_minimap_pixel_size()'; then
  echo "[ux minimap_zoom_scale_red] FAIL: demo rebuild missing constant pixel_size call" >&2
  exit 1
fi
if grep -A40 'minimap_cache = editor_ux_minimap_cache_tick' "$DEMO" | grep -q 'editor_ux_minimap_pixel_size_for_font'; then
  echo "[ux minimap_zoom_scale_red] FAIL: demo already uses for_font (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux minimap_zoom_scale_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok minimap_zoom_scale_stable' "$STABLE"; then
  echo "[ux minimap_zoom_scale_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux minimap_zoom_scale_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok minimap_zoom_scale_red"
echo "[ux minimap_zoom_scale_red] ok" >&2
