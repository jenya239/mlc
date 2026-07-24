#!/usr/bin/env bash
# TRACK_EDITOR_OVERLAY_THEME_TINT — L0: overlay/menu/nav use theme.*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux overlay_theme_tint_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

# Dark triples from STEP=1 gap must be gone.
if grep -q '0.16, 0.18, 0.22' "$DEMO"; then
  echo "[ux overlay_theme_tint_stable] FAIL: dirty-close dark panel RGB still present" >&2
  exit 1
fi
if grep -q '0.18, 0.20, 0.24' "$DEMO"; then
  echo "[ux overlay_theme_tint_stable] FAIL: context-menu dark panel RGB still present" >&2
  exit 1
fi
if grep -q '0.22, 0.24, 0.28' "$DEMO"; then
  echo "[ux overlay_theme_tint_stable] FAIL: context-menu item dark RGB still present" >&2
  exit 1
fi
if grep -q '0.22, 0.24, 0.30' "$DEMO"; then
  echo "[ux overlay_theme_tint_stable] FAIL: nav dark RGB still present" >&2
  exit 1
fi

# Draw sites use theme.panel_* / theme.muted_*.
if ! grep -A8 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.panel_red'; then
  echo "[ux overlay_theme_tint_stable] FAIL: dirty-close missing theme.panel_" >&2
  exit 1
fi
if ! grep -A10 'editor_ux_context_menu_panel_rect' "$DEMO" | grep -q 'theme.panel_red'; then
  echo "[ux overlay_theme_tint_stable] FAIL: context-menu panel missing theme.panel_" >&2
  exit 1
fi
if ! grep -A12 'editor_ux_context_menu_item_rect' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux overlay_theme_tint_stable] FAIL: context-menu item missing theme.muted_" >&2
  exit 1
fi
if ! grep -A6 'nav_back_rect.x, nav_back_rect.y' "$DEMO" | grep -q 'theme.panel_red'; then
  echo "[ux overlay_theme_tint_stable] FAIL: nav back missing theme.panel_" >&2
  exit 1
fi
if ! grep -A6 'nav_forward_rect.x, nav_forward_rect.y' "$DEMO" | grep -q 'theme.panel_red'; then
  echo "[ux overlay_theme_tint_stable] FAIL: nav forward missing theme.panel_" >&2
  exit 1
fi

# Discard danger red kept intentionally.
if ! grep -q '0.55, 0.22, 0.22' "$DEMO"; then
  echo "[ux overlay_theme_tint_stable] FAIL: discard danger red missing" >&2
  exit 1
fi

echo "ux_ok overlay_theme_tint_stable"
echo "[ux overlay_theme_tint_stable] ok" >&2
