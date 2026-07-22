#!/usr/bin/env bash
# TRACK_EDITOR_IDLE_CARET_BLINK STEP=1 — idle path has no blink/paint deadline.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux idle_caret_blink_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Idle early-out present (#38).
if ! grep -q 'glfw_gl_context_wait_events_timeout' "$DEMO"; then
  echo "[ux idle_caret_blink_red] FAIL: demo_live missing idle wait" >&2
  exit 1
fi
if ! grep -q 'if activity == 0 then' "$DEMO"; then
  echo "[ux idle_caret_blink_red] FAIL: demo_live missing activity==0 idle branch" >&2
  exit 1
fi

# Caret blink still keyed off frame_index (freezes when idle continue skips draw).
if ! grep -q 'caret_blink_visible' "$DEMO"; then
  echo "[ux idle_caret_blink_red] FAIL: demo_live missing caret_blink_visible" >&2
  exit 1
fi
if ! grep -A2 'caret_blink_visible' "$DEMO" | grep -q 'frame_index \* 16'; then
  echo "[ux idle_caret_blink_red] FAIL: caret blink not on frame_index (expected gap)" >&2
  exit 1
fi

# Gap: no paint_dirty / blink deadline yet.
if grep -q 'paint_dirty' "$DEMO"; then
  echo "[ux idle_caret_blink_red] FAIL: paint_dirty already present (expected gap)" >&2
  exit 1
fi

echo "ux_ok idle_caret_blink_red"
echo "[ux idle_caret_blink_red] ok" >&2
