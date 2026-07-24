#!/usr/bin/env bash
# TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME — L0: chrome_hover default from Theme.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
HOVER="$ROOT_DIR/misc/editor/ux/chrome_hover.mlc"

if [ ! -f "$HOVER" ]; then
  echo "[ux chrome_hover_default_theme_stable] FAIL: missing $HOVER" >&2
  exit 1
fi

if grep -q '0.933, 0.910, 0.835' "$HOVER"; then
  echo "[ux chrome_hover_default_theme_stable] FAIL: bare Light RGB still present" >&2
  exit 1
fi

if ! grep -qE '^import .*theme_solarized_light' "$HOVER"; then
  echo "[ux chrome_hover_default_theme_stable] FAIL: missing theme_solarized_light import" >&2
  exit 1
fi

if ! grep -A8 'export fn editor_ux_chrome_hover_rgb' "$HOVER" | grep -q 'theme_solarized_light()'; then
  echo "[ux chrome_hover_default_theme_stable] FAIL: default hover missing theme_solarized_light()" >&2
  exit 1
fi

if ! grep -A12 'editor_ux_chrome_hover_draw_report_stub' "$HOVER" | grep -q 'theme.panel_red'; then
  echo "[ux chrome_hover_default_theme_stable] FAIL: stub missing theme.panel_" >&2
  exit 1
fi

echo "ux_ok chrome_hover_default_theme_stable"
echo "[ux chrome_hover_default_theme_stable] ok" >&2
