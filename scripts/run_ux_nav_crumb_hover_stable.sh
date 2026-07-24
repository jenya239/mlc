#!/usr/bin/env bash
# TRACK_EDITOR_NAV_CRUMB_HOVER — L0: nav/crumb use chrome_hover_draw_entry.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux nav_crumb_hover_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'input, breadcrumb_rect, theme.panel_red' "$DEMO"; then
  echo "[ux nav_crumb_hover_stable] FAIL: breadcrumb missing chrome_hover" >&2
  exit 1
fi
if ! grep -q 'input, nav_back_rect, theme.panel_red' "$DEMO"; then
  echo "[ux nav_crumb_hover_stable] FAIL: nav back missing chrome_hover" >&2
  exit 1
fi
if ! grep -q 'input, nav_forward_rect, theme.panel_red' "$DEMO"; then
  echo "[ux nav_crumb_hover_stable] FAIL: nav forward missing chrome_hover" >&2
  exit 1
fi

# Flat .x/.y panel fills for those sites must be gone.
if grep -q 'breadcrumb_rect.x, breadcrumb_rect.y' "$DEMO"; then
  echo "[ux nav_crumb_hover_stable] FAIL: flat breadcrumb panel fill still present" >&2
  exit 1
fi
if grep -q 'nav_back_rect.x, nav_back_rect.y' "$DEMO"; then
  echo "[ux nav_crumb_hover_stable] FAIL: flat nav back panel fill still present" >&2
  exit 1
fi
if grep -q 'nav_forward_rect.x, nav_forward_rect.y' "$DEMO"; then
  echo "[ux nav_crumb_hover_stable] FAIL: flat nav forward panel fill still present" >&2
  exit 1
fi

echo "ux_ok nav_crumb_hover_stable"
echo "[ux nav_crumb_hover_stable] ok" >&2
