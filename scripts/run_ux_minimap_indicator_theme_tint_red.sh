#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT STEP=1 — indicator still hardcoded RGB.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_minimap_indicator_theme_tint_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'editor_ux_minimap_viewport_indicator' "$DEMO"; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: missing minimap indicator draw" >&2
  exit 1
fi

# Gap: indicator still hardcodes 0.35/0.40/0.45.
if ! grep -q '0.35, 0.40, 0.45' "$DEMO"; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: missing hardcoded indicator RGB" >&2
  exit 1
fi

# Draw block must not already use theme.muted_*.
if grep -A10 'editor_ux_minimap_viewport_indicator' "$DEMO" | grep -q 'theme.muted_'; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: indicator already uses theme.muted_ (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok minimap_indicator_theme_tint_stable' "$STABLE"; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux minimap_indicator_theme_tint_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok minimap_indicator_theme_tint_red"
echo "[ux minimap_indicator_theme_tint_red] ok" >&2
