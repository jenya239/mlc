#!/usr/bin/env bash
# TRACK_EDITOR_SCROLLBAR_THUMB_DRAG STEP=1 — thumbs draw; no drag→scroll.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
SCROLL="$ROOT_DIR/misc/editor/ux/scroll.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_scrollbar_thumb_drag_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$SCROLL" ]; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: missing $SCROLL" >&2
  exit 1
fi

# Thumbs drawn.
if ! grep -q 'editor_ux_content_scrollbar_thumb' "$DEMO"; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: missing vertical thumb draw" >&2
  exit 1
fi
if ! grep -q 'editor_ux_content_scrollbar_thumb_x' "$DEMO"; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: missing horizontal thumb draw" >&2
  exit 1
fi

# Gap: no scroll-from-thumb-drag helper / wire yet.
if grep -qE 'scrollbar_thumb_drag|scroll_at_thumb|content_scrollbar_scroll_at' "$DEMO"; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: demo already has thumb-drag wire (expected gap)" >&2
  exit 1
fi
if grep -qE 'scrollbar_thumb_drag|scroll_at_thumb|content_scrollbar_scroll_at' "$SCROLL"; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: scroll.mlc already has thumb-drag helper (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok scrollbar_thumb_drag_stable' "$STABLE"; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux scrollbar_thumb_drag_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok scrollbar_thumb_drag_red"
echo "[ux scrollbar_thumb_drag_red] ok" >&2
