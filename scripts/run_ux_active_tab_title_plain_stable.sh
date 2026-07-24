#!/usr/bin/env bash
# TRACK_EDITOR_ACTIVE_TAB_TITLE_PLAIN — L0: no [title] wraps; selection fill remains.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux active_tab_title_plain_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if grep -qE '= "\[" \+ [a-z_]+ \+ "\]"' "$DEMO"; then
  echo "[ux active_tab_title_plain_stable] FAIL: [title] wraps still present" >&2
  exit 1
fi

if ! grep -A25 'while hover_tab_index < tab_widths.length()' "$DEMO" | grep -q 'theme.selection_red'; then
  echo "[ux active_tab_title_plain_stable] FAIL: active selection fill missing" >&2
  exit 1
fi

echo "ux_ok active_tab_title_plain_stable"
echo "[ux active_tab_title_plain_stable] ok" >&2
