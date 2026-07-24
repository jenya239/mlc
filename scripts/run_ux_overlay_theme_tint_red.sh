#!/usr/bin/env bash
# TRACK_EDITOR_OVERLAY_THEME_TINT STEP=1 — overlay/menu/nav still dark RGB.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_overlay_theme_tint_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux overlay_theme_tint_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'dirty_close_overlay_is_visible' "$DEMO"; then
  echo "[ux overlay_theme_tint_red] FAIL: missing dirty-close draw" >&2
  exit 1
fi
if ! grep -q 'editor_ux_context_menu_panel_rect' "$DEMO"; then
  echo "[ux overlay_theme_tint_red] FAIL: missing context-menu panel draw" >&2
  exit 1
fi
if ! grep -q 'folder_nav_back_rect' "$DEMO"; then
  echo "[ux overlay_theme_tint_red] FAIL: missing nav back draw" >&2
  exit 1
fi

# Gap: hardcoded dark fills at overlay / menu / nav sites.
if ! grep -q '0.16, 0.18, 0.22' "$DEMO"; then
  echo "[ux overlay_theme_tint_red] FAIL: missing dirty-close dark panel RGB" >&2
  exit 1
fi
if ! grep -q '0.18, 0.20, 0.24' "$DEMO"; then
  echo "[ux overlay_theme_tint_red] FAIL: missing context-menu dark panel RGB" >&2
  exit 1
fi
if ! grep -q '0.22, 0.24, 0.28' "$DEMO"; then
  echo "[ux overlay_theme_tint_red] FAIL: missing context-menu item dark RGB" >&2
  exit 1
fi
nav_count="$(grep -c '0.22, 0.24, 0.30' "$DEMO" || true)"
if [ "$nav_count" -lt 2 ]; then
  echo "[ux overlay_theme_tint_red] FAIL: expected ≥2 nav dark fills, got $nav_count" >&2
  exit 1
fi

# Those blocks must not already use theme.panel_*.
if grep -A8 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.panel_'; then
  echo "[ux overlay_theme_tint_red] FAIL: dirty-close already uses theme.panel_ (expected gap)" >&2
  exit 1
fi
if grep -A10 'editor_ux_context_menu_panel_rect' "$DEMO" | grep -q 'theme.panel_'; then
  echo "[ux overlay_theme_tint_red] FAIL: context-menu already uses theme.panel_ (expected gap)" >&2
  exit 1
fi
if grep -A6 'nav_back_rect.x, nav_back_rect.y' "$DEMO" | grep -q 'theme.panel_'; then
  echo "[ux overlay_theme_tint_red] FAIL: nav back already uses theme.panel_ (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux overlay_theme_tint_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok overlay_theme_tint_stable' "$STABLE"; then
  echo "[ux overlay_theme_tint_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux overlay_theme_tint_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok overlay_theme_tint_red"
echo "[ux overlay_theme_tint_red] ok" >&2
