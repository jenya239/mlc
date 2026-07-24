#!/usr/bin/env bash
# TRACK_EDITOR_WRAP_HIT_SCROLL_X STEP=1 — wrap pixel hit still ignores scroll_x.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
WRAP="$ROOT_DIR/misc/editor/layout/word_wrap.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_wrap_hit_scroll_x_stable.sh"

if [ ! -f "$WRAP" ]; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: missing $WRAP" >&2
  exit 1
fi
if [ ! -f "$DEMO" ]; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: wrap_offset_at_point_pixel has scroll_offset_y but no scroll_offset_x.
if ! grep -q 'export fn wrap_offset_at_point_pixel' "$WRAP"; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: missing wrap_offset_at_point_pixel" >&2
  exit 1
fi
if ! awk '/export fn wrap_offset_at_point_pixel/,/^\) -> i32/' "$WRAP" | grep -q 'scroll_offset_y'; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: pixel hit missing scroll_offset_y" >&2
  exit 1
fi
if awk '/export fn wrap_offset_at_point_pixel/,/^\) -> i32/' "$WRAP" | grep -q 'scroll_offset_x'; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: pixel hit already has scroll_offset_x (expected gap)" >&2
  exit 1
fi
# content_x path must not add scroll_offset_x yet.
if grep -A40 'export fn wrap_offset_at_point_pixel' "$WRAP" | grep -q 'scroll_offset_x'; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: pixel hit body already uses scroll_offset_x (expected gap)" >&2
  exit 1
fi

# Demo click wires scroll_offset_y only (no scroll_offset_x / app.ux.scroll_offset_x).
if ! grep -A6 'const click_offset = wrap_offset_at_point_pixel' "$DEMO" | grep -q 'scroll_offset_y'; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: demo click missing scroll_offset_y" >&2
  exit 1
fi
if grep -A6 'const click_offset = wrap_offset_at_point_pixel' "$DEMO" | grep -qE 'scroll_offset_x|app\.ux\.scroll_offset_x'; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: demo click already passes scroll_x (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok wrap_hit_scroll_x_stable|wrap hit scroll_x ok' "$STABLE"; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux wrap_hit_scroll_x_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok wrap_hit_scroll_x_red"
echo "[ux wrap_hit_scroll_x_red] ok" >&2
