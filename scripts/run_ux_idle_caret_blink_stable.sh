#!/usr/bin/env bash
# TRACK_EDITOR_IDLE_CARET_BLINK — paint_dirty blink deadline (green after STEP=2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux idle_caret_blink_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'paint_dirty' "$DEMO"; then
  echo "[ux idle_caret_blink_stable] FAIL: paint_dirty missing (expected red until STEP=2)" >&2
  exit 1
fi
if ! grep -q 'clock_ms' "$DEMO"; then
  echo "[ux idle_caret_blink_stable] FAIL: clock_ms missing" >&2
  exit 1
fi
if grep -A2 'caret_blink_visible' "$DEMO" | grep -q 'frame_index \* 16'; then
  echo "[ux idle_caret_blink_stable] FAIL: caret blink still on frame_index" >&2
  exit 1
fi
if ! grep -A2 'caret_blink_visible' "$DEMO" | grep -q 'clock_ms'; then
  echo "[ux idle_caret_blink_stable] FAIL: caret_blink_visible not driven by clock_ms" >&2
  exit 1
fi

echo "ux_ok idle_caret_blink_stable"
echo "[ux idle_caret_blink_stable] ok" >&2
