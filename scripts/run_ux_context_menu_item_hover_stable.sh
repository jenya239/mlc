#!/usr/bin/env bash
# TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER — L0: menu items use chrome_hover_draw_entry.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux context_menu_item_hover_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -A12 'editor_ux_context_menu_item_rect' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux context_menu_item_hover_stable] FAIL: menu items missing chrome_hover" >&2
  exit 1
fi

# Muted base still passed into chrome_hover.
if ! grep -A12 'editor_ux_context_menu_item_rect' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux context_menu_item_hover_stable] FAIL: menu items missing muted base" >&2
  exit 1
fi

echo "ux_ok context_menu_item_hover_stable"
echo "[ux context_menu_item_hover_stable] ok" >&2
