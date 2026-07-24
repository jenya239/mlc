#!/usr/bin/env bash
# TRACK_EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG STEP=1 — folder thumb draws; no drag.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
FOLDER="$ROOT_DIR/misc/editor/ux/folder_panel.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_folder_scrollbar_thumb_drag_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$FOLDER" ]; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: missing $FOLDER" >&2
  exit 1
fi

if ! grep -q 'folder_scrollbar_thumb' "$DEMO"; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: missing folder thumb draw" >&2
  exit 1
fi

# Gap: no folder thumb-drag helper / wire yet.
if grep -qE 'folder_scrollbar_scroll_at|folder_scrollbar_thumb_drag|folder_scrollbar_track' "$DEMO"; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: demo already has folder thumb-drag wire (expected gap)" >&2
  exit 1
fi
if grep -qE 'folder_scrollbar_scroll_at|folder_scrollbar_thumb_drag|folder_scrollbar_track' "$FOLDER"; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: folder_panel already has scroll_at helper (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok folder_scrollbar_thumb_drag_stable' "$STABLE"; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux folder_scrollbar_thumb_drag_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok folder_scrollbar_thumb_drag_red"
echo "[ux folder_scrollbar_thumb_drag_red] ok" >&2
