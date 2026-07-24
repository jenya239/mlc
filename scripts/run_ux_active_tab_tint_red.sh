#!/usr/bin/env bash
# TRACK_EDITOR_ACTIVE_TAB_TINT STEP=1 — active tab still same panel idle fill.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_active_tab_tint_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux active_tab_tint_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'hover_tab_index' "$DEMO"; then
  echo "[ux active_tab_tint_red] FAIL: missing tab fill loop" >&2
  exit 1
fi

# Gap: tab fill always uses theme.panel_* (no active selection_* branch).
if ! grep -A12 'while hover_tab_index < tab_widths.length()' "$DEMO" | grep -q 'theme.panel_red'; then
  echo "[ux active_tab_tint_red] FAIL: tab fill missing theme.panel_" >&2
  exit 1
fi

if grep -A20 'while hover_tab_index < tab_widths.length()' "$DEMO" | grep -q 'theme.selection_'; then
  echo "[ux active_tab_tint_red] FAIL: tab fill already uses theme.selection_ (expected gap)" >&2
  exit 1
fi

# Active mark is still text brackets only.
if ! grep -q 'draw_title = "\[" + draw_title + "\]"' "$DEMO"; then
  echo "[ux active_tab_tint_red] FAIL: missing [title] active text mark" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux active_tab_tint_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok active_tab_tint_stable' "$STABLE"; then
  echo "[ux active_tab_tint_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux active_tab_tint_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok active_tab_tint_red"
echo "[ux active_tab_tint_red] ok" >&2
