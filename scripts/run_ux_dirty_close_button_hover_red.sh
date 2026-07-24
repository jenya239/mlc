#!/usr/bin/env bash
# TRACK_EDITOR_DIRTY_CLOSE_BUTTON_HOVER STEP=1 — discard/cancel still flat fills.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_dirty_close_button_hover_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux dirty_close_button_hover_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'dirty_close_overlay_is_visible(overlay) then' "$DEMO"; then
  echo "[ux dirty_close_button_hover_red] FAIL: missing dirty-close draw" >&2
  exit 1
fi

# Gap: discard/cancel paint flat theme colors without chrome_hover.
if ! grep -A16 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.danger_red'; then
  echo "[ux dirty_close_button_hover_red] FAIL: missing flat danger discard fill" >&2
  exit 1
fi
if ! grep -A16 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.accent_red'; then
  echo "[ux dirty_close_button_hover_red] FAIL: missing flat accent cancel fill" >&2
  exit 1
fi
if grep -A16 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux dirty_close_button_hover_red] FAIL: dirty-close buttons already use chrome_hover (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux dirty_close_button_hover_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok dirty_close_button_hover_stable' "$STABLE"; then
  echo "[ux dirty_close_button_hover_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux dirty_close_button_hover_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok dirty_close_button_hover_red"
echo "[ux dirty_close_button_hover_red] ok" >&2
