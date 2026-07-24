#!/usr/bin/env bash
# TRACK_EDITOR_WRAP_SELECTION_VISUAL STEP=1 — selection fill still doc-line.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_wrap_selection_visual_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux wrap_selection_visual_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: selection maps doc line → viewport row, not visual wrap rows.
if ! grep -q 'highlight_line - visible.first_line' "$DEMO"; then
  echo "[ux wrap_selection_visual_red] FAIL: demo_live missing doc-line row map" >&2
  exit 1
fi
if ! grep -q 'let mut highlight_line = start_position.line' "$DEMO"; then
  echo "[ux wrap_selection_visual_red] FAIL: demo_live missing doc-line selection loop" >&2
  exit 1
fi
# No visual-row selection helper yet.
if grep -qE 'selection_fill_visual_rows|wrap_selection_visual|intersect_selection_visual' "$DEMO"; then
  echo "[ux wrap_selection_visual_red] FAIL: demo_live already has visual selection (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux wrap_selection_visual_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok wrap_selection_visual_stable|wrap selection visual ok' "$STABLE"; then
  echo "[ux wrap_selection_visual_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux wrap_selection_visual_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok wrap_selection_visual_red"
echo "[ux wrap_selection_visual_red] ok" >&2
