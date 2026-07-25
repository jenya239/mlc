#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b minimap_cache STEP=1 — minimap cache still bare in demo_live.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_frame_layout_minimap_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_minimap_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if [ ! -f "$LAYOUT" ]; then
  echo "[editor frame_layout_minimap_red] FAIL: missing $LAYOUT (prior slice prerequisite)" >&2
  exit 1
fi

# Gap: no minimap API on frame_layout yet.
if grep -q 'frame_layout_tick_minimap' "$LAYOUT"; then
  echo "[editor frame_layout_minimap_red] FAIL: frame_layout_tick_minimap already present (expected gap)" >&2
  exit 1
fi
if grep -q 'frame_layout_tick_minimap' "$DEMO"; then
  echo "[editor frame_layout_minimap_red] FAIL: demo_live already uses frame_layout_tick_minimap (expected gap)" >&2
  exit 1
fi
if grep -q 'minimap_cache: MinimapCacheState' "$LAYOUT"; then
  echo "[editor frame_layout_minimap_red] FAIL: EditorFrameLayout already has minimap_cache (expected gap)" >&2
  exit 1
fi

# Gap: bare minimap_cache local + direct tick in demo_live.
if ! grep -q 'let mut minimap_cache = editor_ux_minimap_cache_new(' "$DEMO"; then
  echo "[editor frame_layout_minimap_red] FAIL: missing bare minimap_cache local (expected gap)" >&2
  exit 1
fi

tick_count="$(grep -c 'editor_ux_minimap_cache_tick(' "$DEMO" || true)"
if [ "$tick_count" -lt 1 ]; then
  echo "[editor frame_layout_minimap_red] FAIL: expected ≥1 editor_ux_minimap_cache_tick( in demo_live, got $tick_count" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor frame_layout_minimap_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok frame_layout_minimap$' "$STABLE"; then
  echo "[editor frame_layout_minimap_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor frame_layout_minimap_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok frame_layout_minimap_red"
echo "[editor frame_layout_minimap_red] ok ticks=$tick_count" >&2
