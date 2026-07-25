#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97c §96 STEP=1 — no green L0 harness yet.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_wheel_hover_focus_independent_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Behavior already correct — gap is missing protective harness.
wheel_block="$(
  awk '/const scroll_ticks = f64_to_i32\(pending_scroll_y/,/visible = visible_line_range\(line_count, app\.ux\.scroll_offset_y/' "$DEMO"
)"
if ! printf '%s\n' "$wheel_block" | grep -q 'tree_hovered'; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: wheel block missing tree_hovered gate" >&2
  exit 1
fi
if ! printf '%s\n' "$wheel_block" | grep -q 'point_in_rect(point_new(input.mouse_x, input.mouse_y), editor_rect)'; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: wheel block missing editor_rect hover gate" >&2
  exit 1
fi
if printf '%s\n' "$wheel_block" | grep -q 'editor_focused'; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: wheel block already gates on editor_focused (unexpected)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok wheel_hover_focus_independent$' "$STABLE"; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux wheel_hover_focus_independent_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok wheel_hover_focus_independent_red"
echo "[ux wheel_hover_focus_independent_red] ok (harness gap; demo already hover-only)" >&2
