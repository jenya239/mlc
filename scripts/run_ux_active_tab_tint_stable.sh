#!/usr/bin/env bash
# TRACK_EDITOR_ACTIVE_TAB_TINT — L0: active tab idle uses theme.selection_*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux active_tab_tint_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'hover_tab_index' "$DEMO"; then
  echo "[ux active_tab_tint_stable] FAIL: missing tab fill loop" >&2
  exit 1
fi

# Active branch uses selection_*; inactive default keeps panel_*.
if ! grep -A25 'while hover_tab_index < tab_widths.length()' "$DEMO" | grep -q 'theme.selection_red'; then
  echo "[ux active_tab_tint_stable] FAIL: active tab missing theme.selection_" >&2
  exit 1
fi

if ! grep -A25 'while hover_tab_index < tab_widths.length()' "$DEMO" | grep -q 'theme.panel_red'; then
  echo "[ux active_tab_tint_stable] FAIL: inactive tab missing theme.panel_" >&2
  exit 1
fi

if ! grep -A25 'while hover_tab_index < tab_widths.length()' "$DEMO" | grep -q 'tabs.active_index'; then
  echo "[ux active_tab_tint_stable] FAIL: missing active_index branch" >&2
  exit 1
fi

echo "ux_ok active_tab_tint_stable"
echo "[ux active_tab_tint_stable] ok" >&2
