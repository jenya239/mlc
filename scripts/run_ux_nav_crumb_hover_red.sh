#!/usr/bin/env bash
# TRACK_EDITOR_NAV_CRUMB_HOVER STEP=1 — nav/crumb still flat panel fills.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_nav_crumb_hover_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux nav_crumb_hover_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'nav_back_rect.x, nav_back_rect.y' "$DEMO"; then
  echo "[ux nav_crumb_hover_red] FAIL: missing nav back draw" >&2
  exit 1
fi
if ! grep -q 'nav_forward_rect.x, nav_forward_rect.y' "$DEMO"; then
  echo "[ux nav_crumb_hover_red] FAIL: missing nav forward draw" >&2
  exit 1
fi
if ! grep -q 'breadcrumb_rect.x, breadcrumb_rect.y' "$DEMO"; then
  echo "[ux nav_crumb_hover_red] FAIL: missing breadcrumb draw" >&2
  exit 1
fi

# Gap: those sites draw flat panel without chrome_hover_draw_entry.
if grep -B6 'nav_back_rect.x, nav_back_rect.y' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux nav_crumb_hover_red] FAIL: nav back already uses chrome_hover (expected gap)" >&2
  exit 1
fi
if grep -B6 'nav_forward_rect.x, nav_forward_rect.y' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux nav_crumb_hover_red] FAIL: nav forward already uses chrome_hover (expected gap)" >&2
  exit 1
fi
if grep -B6 'breadcrumb_rect.x, breadcrumb_rect.y' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux nav_crumb_hover_red] FAIL: breadcrumb already uses chrome_hover (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux nav_crumb_hover_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok nav_crumb_hover_stable' "$STABLE"; then
  echo "[ux nav_crumb_hover_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux nav_crumb_hover_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok nav_crumb_hover_red"
echo "[ux nav_crumb_hover_red] ok" >&2
