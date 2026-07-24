#!/usr/bin/env bash
# TRACK_EDITOR_ACTIVE_TAB_TITLE_PLAIN STEP=1 — [title] brackets still present.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_active_tab_title_plain_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux active_tab_title_plain_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: three active-title bracket wraps.
bracket_count="$(grep -cE '= "\[" \+ [a-z_]+ \+ "\]"' "$DEMO" || true)"
if [ "$bracket_count" -lt 3 ]; then
  echo "[ux active_tab_title_plain_red] FAIL: expected ≥3 [title] wraps, got $bracket_count" >&2
  exit 1
fi

# Selection fill must still be present (active mark stays after STEP=2).
if ! grep -q 'theme.selection_red' "$DEMO"; then
  echo "[ux active_tab_title_plain_red] FAIL: missing theme.selection_ active fill" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux active_tab_title_plain_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok active_tab_title_plain_stable' "$STABLE"; then
  echo "[ux active_tab_title_plain_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux active_tab_title_plain_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok active_tab_title_plain_red"
echo "[ux active_tab_title_plain_red] ok" >&2
