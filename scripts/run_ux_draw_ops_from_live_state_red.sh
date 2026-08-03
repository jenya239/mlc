#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107q — Red: live paint not via EditorPaintOp.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_draw_ops_from_live_state.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PAINT_OPS="$ROOT_DIR/misc/editor/ux/paint_ops.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_draw_ops_from_live_state.sh"

fail() {
  echo "[ux draw_ops_from_live_state_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"

# Red: product still paints chrome bands with direct solid_renderer_rect.
if ! grep -q 'solid_renderer_begin' "$DEMO"; then
  fail "demo_live missing solid_renderer_begin (expected today's paint path)"
fi
if ! grep -q 'solid_renderer_rect' "$DEMO"; then
  fail "demo_live missing solid_renderer_rect (expected today's gap)"
fi

# Red: no editor-local paint-ops module / flatten yet.
if [ -f "$PAINT_OPS" ]; then
  fail "paint_ops.mlc already present (expected gap until Green q1)"
fi
if grep -q 'EditorPaintOp' "$DEMO"; then
  fail "EditorPaintOp already referenced in demo_live (expected gap until Green q1)"
fi
if grep -q 'editor_paint_ops_flatten' "$DEMO"; then
  fail "editor_paint_ops_flatten already present (expected gap until Green q1)"
fi

# Red: demo does not import paint_ops / draw_frame for live paint.
if grep -q "from './ux/paint_ops'" "$DEMO"; then
  fail "demo_live already imports paint_ops (expected gap until Green q1)"
fi
if grep -q 'editor_ux_draw_frame' "$DEMO"; then
  fail "demo_live already calls editor_ux_draw_frame (unexpected)"
fi

# Red: green gate artifact must be absent.
if [ -f "$STABLE" ]; then
  fail "green gate already present (expected gap until Green q1)"
fi

fail "live paint still direct solid_renderer_rect; no EditorPaintOp flatten (Red expected)"
