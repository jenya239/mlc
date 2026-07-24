#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_WIDTH_ZOOM STEP=1 — strip width still fixed constant.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_minimap_width_zoom_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_width_zoom_red] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$MINIMAP" ]; then
  echo "[ux minimap_width_zoom_red] FAIL: missing $MINIMAP" >&2
  exit 1
fi

# Gap: constant width + bare layout; no width_for_font / layout_for_font.
if ! grep -q 'export fn editor_ux_minimap_width()' "$MINIMAP"; then
  echo "[ux minimap_width_zoom_red] FAIL: missing constant width helper" >&2
  exit 1
fi
if grep -q 'editor_ux_minimap_width_for_font' "$MINIMAP"; then
  echo "[ux minimap_width_zoom_red] FAIL: width_for_font already present (expected gap)" >&2
  exit 1
fi
if grep -q 'editor_ux_minimap_layout_for_font' "$MINIMAP"; then
  echo "[ux minimap_width_zoom_red] FAIL: layout_for_font already present (expected gap)" >&2
  exit 1
fi
if ! grep -q 'editor_ux_minimap_layout(content_band)' "$DEMO"; then
  echo "[ux minimap_width_zoom_red] FAIL: demo missing bare minimap_layout(content_band)" >&2
  exit 1
fi
if grep -q 'editor_ux_minimap_layout_for_font' "$DEMO"; then
  echo "[ux minimap_width_zoom_red] FAIL: demo already uses layout_for_font (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux minimap_width_zoom_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok minimap_width_zoom_stable' "$STABLE"; then
  echo "[ux minimap_width_zoom_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux minimap_width_zoom_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok minimap_width_zoom_red"
echo "[ux minimap_width_zoom_red] ok" >&2
