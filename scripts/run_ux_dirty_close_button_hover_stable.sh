#!/usr/bin/env bash
# TRACK_EDITOR_DIRTY_CLOSE_BUTTON_HOVER — L0: discard/cancel use chrome_hover.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux dirty_close_button_hover_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -A24 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux dirty_close_button_hover_stable] FAIL: dirty-close buttons missing chrome_hover" >&2
  exit 1
fi

# Danger/accent bases still passed into chrome_hover.
if ! grep -A24 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.danger_red'; then
  echo "[ux dirty_close_button_hover_stable] FAIL: discard missing danger base" >&2
  exit 1
fi
if ! grep -A24 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.accent_red'; then
  echo "[ux dirty_close_button_hover_stable] FAIL: cancel missing accent base" >&2
  exit 1
fi

# Prefer rect helpers over hardcoded button geometry.
if ! grep -A24 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'dirty_close_overlay_discard_rect'; then
  echo "[ux dirty_close_button_hover_stable] FAIL: missing discard_rect helper" >&2
  exit 1
fi
if ! grep -A24 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'dirty_close_overlay_cancel_rect'; then
  echo "[ux dirty_close_button_hover_stable] FAIL: missing cancel_rect helper" >&2
  exit 1
fi

echo "ux_ok dirty_close_button_hover_stable"
echo "[ux dirty_close_button_hover_stable] ok" >&2
