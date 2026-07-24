#!/usr/bin/env bash
# TRACK_EDITOR_DISCARD_DANGER_THEME STEP=1 — discard button still bare RGB.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
THEME="$ROOT_DIR/misc/editor/ui/theme.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_discard_danger_theme_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux discard_danger_theme_red] FAIL: missing $DEMO" >&2
  exit 1
fi
if [ ! -f "$THEME" ]; then
  echo "[ux discard_danger_theme_red] FAIL: missing $THEME" >&2
  exit 1
fi

if ! grep -q 'dirty_close_overlay_is_visible' "$DEMO"; then
  echo "[ux discard_danger_theme_red] FAIL: missing dirty-close draw" >&2
  exit 1
fi

# Gap: discard button still hardcodes 0.55/0.22/0.22.
if ! grep -q '0.55, 0.22, 0.22' "$DEMO"; then
  echo "[ux discard_danger_theme_red] FAIL: missing hardcoded discard danger RGB" >&2
  exit 1
fi

# Draw block must not already use theme.danger_*.
if grep -A12 'dirty_close_overlay_is_visible(overlay) then' "$DEMO" | grep -q 'theme.danger_'; then
  echo "[ux discard_danger_theme_red] FAIL: discard already uses theme.danger_ (expected gap)" >&2
  exit 1
fi

# Theme must not already declare danger_*.
if grep -q 'danger_red' "$THEME"; then
  echo "[ux discard_danger_theme_red] FAIL: Theme already has danger_red (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux discard_danger_theme_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok discard_danger_theme_stable' "$STABLE"; then
  echo "[ux discard_danger_theme_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux discard_danger_theme_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok discard_danger_theme_red"
echo "[ux discard_danger_theme_red] ok" >&2
