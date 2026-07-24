#!/usr/bin/env bash
# TRACK_EDITOR_MULTI_SELECTION_VISUAL STEP=1 — fill still primary selection only.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_multi_selection_visual_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux multi_selection_visual_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: fill uses primary selection only (not multi ranges loop).
if ! grep -q 'selection_fill_visual_rows' "$DEMO"; then
  echo "[ux multi_selection_visual_red] FAIL: demo_live missing selection_fill_visual_rows" >&2
  exit 1
fi
if ! grep -q 'if !selection_is_empty(selection) then' "$DEMO"; then
  echo "[ux multi_selection_visual_red] FAIL: demo_live missing primary selection fill gate" >&2
  exit 1
fi
# No multi-range fill loop yet.
if grep -qE 'multi_selection_fill|fill_spans_for_multi|selection_fill_visual_rows\([^)]*multi' "$DEMO"; then
  echo "[ux multi_selection_visual_red] FAIL: demo_live already has multi fill (expected gap)" >&2
  exit 1
fi
# Prove fill block does not iterate multi ranges.
if grep -A20 'const fill_spans = selection_fill_visual_rows' "$DEMO" | grep -q 'multi_selection_count\|multi\.ranges\|for.*multi'; then
  echo "[ux multi_selection_visual_red] FAIL: fill already walks multi (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux multi_selection_visual_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok multi_selection_visual_stable|multi selection visual ok' "$STABLE"; then
  echo "[ux multi_selection_visual_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux multi_selection_visual_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok multi_selection_visual_red"
echo "[ux multi_selection_visual_red] ok" >&2
