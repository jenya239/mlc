#!/usr/bin/env bash
# ARCH-LINT: source-shape / ownership only (TRACK_EDITOR_HYGIENE §107r). Not a behavioural UX scenario.
# TRACK_EDITOR_RENDER_ARCHITECTURE §97c §96 STEP=2 — L0: wheel follows hover, not focus.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux wheel_hover_focus_independent_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

wheel_block="$(
  awk '/const scroll_ticks = f64_to_i32\(pending_scroll_y/,/visible = visible_line_range\(line_count, app\.ux\.scroll_offset_y/' "$DEMO"
)"
if [ -z "$wheel_block" ]; then
  echo "[ux wheel_hover_focus_independent_stable] FAIL: wheel block not found" >&2
  exit 1
fi

if ! printf '%s\n' "$wheel_block" | grep -q 'tree_hovered'; then
  echo "[ux wheel_hover_focus_independent_stable] FAIL: missing tree_hovered gate" >&2
  exit 1
fi
if ! printf '%s\n' "$wheel_block" | grep -q 'point_in_rect(point_new(input.mouse_x, input.mouse_y), editor_rect)'; then
  echo "[ux wheel_hover_focus_independent_stable] FAIL: missing editor_rect hover gate" >&2
  exit 1
fi
if ! printf '%s\n' "$wheel_block" | grep -q 'editor_app_wheel_scroll('; then
  echo "[ux wheel_hover_focus_independent_stable] FAIL: missing editor_app_wheel_scroll in wheel block" >&2
  exit 1
fi
if printf '%s\n' "$wheel_block" | grep -q 'editor_focused'; then
  echo "[ux wheel_hover_focus_independent_stable] FAIL: wheel block must not reference editor_focused" >&2
  exit 1
fi

echo "ux_ok wheel_hover_focus_independent"
echo "[ux wheel_hover_focus_independent_stable] ok" >&2
