#!/usr/bin/env bash
# TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME STEP=1 — chrome_hover bare Light RGB.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
HOVER="$ROOT_DIR/misc/editor/ux/chrome_hover.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_chrome_hover_default_theme_stable.sh"

if [ ! -f "$HOVER" ]; then
  echo "[ux chrome_hover_default_theme_red] FAIL: missing $HOVER" >&2
  exit 1
fi

if ! grep -q 'editor_ux_chrome_hover_rgb' "$HOVER"; then
  echo "[ux chrome_hover_default_theme_red] FAIL: missing default hover helper" >&2
  exit 1
fi

# Gap: bare Solarized Light panel RGB still embedded.
bare_count="$(grep -c '0.933, 0.910, 0.835' "$HOVER" || true)"
if [ "$bare_count" -lt 2 ]; then
  echo "[ux chrome_hover_default_theme_red] FAIL: expected ≥2 bare Light RGB sites, got $bare_count" >&2
  exit 1
fi

# Default path must not already import theme_solarized_light.
if grep -qE '^import .*theme_solarized_light' "$HOVER"; then
  echo "[ux chrome_hover_default_theme_red] FAIL: chrome_hover already imports theme_solarized_light (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux chrome_hover_default_theme_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok chrome_hover_default_theme_stable' "$STABLE"; then
  echo "[ux chrome_hover_default_theme_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux chrome_hover_default_theme_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok chrome_hover_default_theme_red"
echo "[ux chrome_hover_default_theme_red] ok" >&2
