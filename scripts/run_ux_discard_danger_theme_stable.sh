#!/usr/bin/env bash
# TRACK_EDITOR_DISCARD_DANGER_THEME — L0: discard uses theme.danger_*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
THEME="$ROOT_DIR/misc/editor/ui/theme.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux discard_danger_theme_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$THEME" ]; then
  echo "[ux discard_danger_theme_stable] FAIL: missing $THEME" >&2
  exit 1
fi

if grep -q '0.55, 0.22, 0.22' "$DEMO"; then
  echo "[ux discard_danger_theme_stable] FAIL: bare discard RGB still present" >&2
  exit 1
fi

if ! grep -q 'danger_red' "$THEME"; then
  echo "[ux discard_danger_theme_stable] FAIL: Theme missing danger_red" >&2
  exit 1
fi

if ! grep -A12 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.danger_red'; then
  echo "[ux discard_danger_theme_stable] FAIL: discard missing theme.danger_" >&2
  exit 1
fi

echo "ux_ok discard_danger_theme_stable"
echo "[ux discard_danger_theme_stable] ok" >&2
