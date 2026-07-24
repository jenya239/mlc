#!/usr/bin/env bash
# TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER STEP=1 — menu items still flat muted fills.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_context_menu_item_hover_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux context_menu_item_hover_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'editor_ux_context_menu_item_rect' "$DEMO"; then
  echo "[ux context_menu_item_hover_red] FAIL: missing menu item draw" >&2
  exit 1
fi

# Gap: item loop paints flat muted without chrome_hover.
if ! grep -A10 'editor_ux_context_menu_item_rect' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux context_menu_item_hover_red] FAIL: missing flat muted item fill" >&2
  exit 1
fi
if grep -A12 'editor_ux_context_menu_item_rect' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux context_menu_item_hover_red] FAIL: menu items already use chrome_hover (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux context_menu_item_hover_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok context_menu_item_hover_stable' "$STABLE"; then
  echo "[ux context_menu_item_hover_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux context_menu_item_hover_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok context_menu_item_hover_red"
echo "[ux context_menu_item_hover_red] ok" >&2
