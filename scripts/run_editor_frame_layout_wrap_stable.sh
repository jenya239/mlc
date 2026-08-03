#!/usr/bin/env bash
# ARCH-LINT: source-shape / ownership only (TRACK_EDITOR_HYGIENE §107r). Not a behavioural UX scenario.
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b STEP=2 — wrap owned by app/frame_layout.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"

if [ ! -f "$LAYOUT" ]; then
  echo "[editor frame_layout_wrap_stable] FAIL: missing $LAYOUT" >&2
  exit 1
fi
if ! grep -q 'export type EditorFrameLayout' "$LAYOUT"; then
  echo "[editor frame_layout_wrap_stable] FAIL: missing EditorFrameLayout" >&2
  exit 1
fi
if ! grep -q 'export fn frame_layout_tick_pixel' "$LAYOUT"; then
  echo "[editor frame_layout_wrap_stable] FAIL: missing frame_layout_tick_pixel" >&2
  exit 1
fi

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_wrap_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if ! grep -q "from './app/frame_layout'" "$DEMO"; then
  echo "[editor frame_layout_wrap_stable] FAIL: demo_live missing frame_layout import" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_pixel(' "$DEMO"; then
  echo "[editor frame_layout_wrap_stable] FAIL: demo_live missing frame_layout_tick_pixel(" >&2
  exit 1
fi

tick_count="$(grep -c 'wrap_count_cache_tick_pixel(' "$DEMO" || true)"
if [ "$tick_count" -ne 0 ]; then
  echo "[editor frame_layout_wrap_stable] FAIL: demo_live still has wrap_count_cache_tick_pixel( (count=$tick_count)" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok frame_layout_wrap"
echo "[editor frame_layout_wrap_stable] ok" >&2
