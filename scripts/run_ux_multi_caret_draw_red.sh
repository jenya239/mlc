#!/usr/bin/env bash
# TRACK_EDITOR_MULTI_CARET_DRAW STEP=1 — caret still primary selection only.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_multi_caret_draw_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux multi_caret_draw_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: caret draw uses primary selection_caret(selection) / last_caret_*.
if ! grep -q 'selection_caret(selection)' "$DEMO"; then
  echo "[ux multi_caret_draw_red] FAIL: demo_live missing primary selection_caret" >&2
  exit 1
fi
if ! grep -q 'last_caret_x' "$DEMO"; then
  echo "[ux multi_caret_draw_red] FAIL: demo_live missing last_caret_x" >&2
  exit 1
fi
# No multi-caret draw loop yet.
if grep -qE 'multi_caret_draw|caret_byte = selection_caret\(multi|while multi_index.*caret' "$DEMO"; then
  echo "[ux multi_caret_draw_red] FAIL: demo_live already has multi caret draw (expected gap)" >&2
  exit 1
fi
if grep -A30 'const caret_byte = selection_caret(selection)' "$DEMO" | grep -q 'multi_selection_count'; then
  echo "[ux multi_caret_draw_red] FAIL: caret already walks multi (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux multi_caret_draw_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok multi_caret_draw_stable|multi caret draw ok' "$STABLE"; then
  echo "[ux multi_caret_draw_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux multi_caret_draw_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok multi_caret_draw_red"
echo "[ux multi_caret_draw_red] ok" >&2
