#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT — L0: indicator uses theme.muted_*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_indicator_theme_tint_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if grep -q '0.35, 0.40, 0.45' "$DEMO"; then
  echo "[ux minimap_indicator_theme_tint_stable] FAIL: hardcoded 0.35/0.40/0.45 still present" >&2
  exit 1
fi

if ! grep -A12 'editor_ux_minimap_viewport_indicator' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux minimap_indicator_theme_tint_stable] FAIL: indicator missing theme.muted_" >&2
  exit 1
fi

echo "ux_ok minimap_indicator_theme_tint_stable"
echo "[ux minimap_indicator_theme_tint_stable] ok" >&2
