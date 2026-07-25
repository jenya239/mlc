#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b minimap_cache STEP=2 — minimap owned by frame_layout.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"

if [ ! -f "$LAYOUT" ]; then
  echo "[editor frame_layout_minimap_stable] FAIL: missing $LAYOUT" >&2
  exit 1
fi
if ! grep -q 'export fn frame_layout_tick_minimap' "$LAYOUT"; then
  echo "[editor frame_layout_minimap_stable] FAIL: missing frame_layout_tick_minimap" >&2
  exit 1
fi
if ! grep -q 'minimap_cache: MinimapCacheState' "$LAYOUT"; then
  echo "[editor frame_layout_minimap_stable] FAIL: EditorFrameLayout missing minimap_cache" >&2
  exit 1
fi

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_minimap_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_minimap(' "$DEMO"; then
  echo "[editor frame_layout_minimap_stable] FAIL: demo_live missing frame_layout_tick_minimap(" >&2
  exit 1
fi
if grep -q 'let mut minimap_cache = editor_ux_minimap_cache_new(' "$DEMO"; then
  echo "[editor frame_layout_minimap_stable] FAIL: demo_live still has bare minimap_cache local" >&2
  exit 1
fi

tick_count="$(grep -c 'editor_ux_minimap_cache_tick(' "$DEMO" || true)"
if [ "$tick_count" -ne 0 ]; then
  echo "[editor frame_layout_minimap_stable] FAIL: demo_live still has editor_ux_minimap_cache_tick( (count=$tick_count)" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok frame_layout_minimap"
echo "[editor frame_layout_minimap_stable] ok" >&2
