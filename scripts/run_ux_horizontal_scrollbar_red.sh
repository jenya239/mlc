#!/usr/bin/env bash
# TRACK_EDITOR_HORIZONTAL_SCROLLBAR STEP=1 — vertical thumb only; no X thumb.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
SCROLL="$ROOT_DIR/misc/editor/ux/scroll.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_horizontal_scrollbar_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux horizontal_scrollbar_red] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$SCROLL" ]; then
  echo "[ux horizontal_scrollbar_red] FAIL: missing $SCROLL" >&2
  exit 1
fi

# Vertical thumb present in demo.
if ! grep -q 'editor_ux_content_scrollbar_thumb' "$DEMO"; then
  echo "[ux horizontal_scrollbar_red] FAIL: missing vertical content thumb call" >&2
  exit 1
fi

# Gap: no horizontal thumb helper / draw yet.
if grep -qE 'content_scrollbar_thumb_x|horizontal_scrollbar|scrollbar_thumb_horizontal' "$DEMO"; then
  echo "[ux horizontal_scrollbar_red] FAIL: demo already has horizontal thumb (expected gap)" >&2
  exit 1
fi
if grep -qE 'content_scrollbar_thumb_x|horizontal_scrollbar|scrollbar_thumb_horizontal' "$SCROLL"; then
  echo "[ux horizontal_scrollbar_red] FAIL: scroll.mlc already has horizontal thumb helper (expected gap)" >&2
  exit 1
fi

# Stable stub still red.
if [ ! -f "$STABLE" ]; then
  echo "[ux horizontal_scrollbar_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok horizontal_scrollbar_stable' "$STABLE"; then
  echo "[ux horizontal_scrollbar_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux horizontal_scrollbar_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok horizontal_scrollbar_red"
echo "[ux horizontal_scrollbar_red] ok" >&2
