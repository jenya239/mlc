#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110c — Red: no paint-list harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_paint_list.sh + EditorPaintList submit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_paint_list.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
EDITOR_DIR="$ROOT_DIR/misc/editor"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
OWNERSHIP="$ROOT_DIR/scripts/run_editor_frame_ownership.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"

fail() {
  echo "[editor_paint_list_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -d "$EDITOR_DIR" ] || fail "missing misc/editor (unexpected drift)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$OWNERSHIP" ] || fail "missing ownership harness (unexpected drift — §110b required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green paint-list harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_paint_list.sh already present (expected gap until Green)"
fi

# Red: no EditorPaintList / editor_paint_list_submit (EditorPaintOp predates this STEP).
if grep -R --include='*.mlc' -n -E 'type[[:space:]]+EditorPaintList([^A-Za-z0-9_]|$)|editor_paint_list_submit' \
  "$EDITOR_DIR" >/dev/null 2>&1; then
  fail "editor sources already declare EditorPaintList/submit (expected until Green)"
fi

# Red: demo_live still has direct load-bearing GL draw/scissor sites.
direct=0
grep -q 'solid_renderer_rect(' "$DEMO" && direct=1
grep -q 'static_text_glyph_batch_draw(' "$DEMO" && direct=1
grep -q 'static_text_draw_lines_colored(' "$DEMO" && direct=1
grep -q 'editor_gl_scissor_enable_rect(' "$DEMO" && direct=1
if [ "$direct" -eq 0 ]; then
  fail "demo_live already has zero direct GL draw/scissor sites (expected gap until Green)"
fi

# Red: ux_gate still does not wire paint-list harness.
if grep -q 'run_editor_paint_list\|EditorPaintList\|editor_paint_list_submit' "$UX_GATE"; then
  fail "run_ux_gate already wires paint list (Decision: not folded)"
fi

# Red: no §110c Green measured paste yet (do not match §110a/§110b ### Green measured).
if grep -A40 '### §110c Green measured\|Green measured (§110c)\|### Green measured (§110c)' "$TRACK" 2>/dev/null | \
  grep -Eq 'paint_ops=|gl_call_from_widget='; then
  fail "TRACK already has paint-list post-cut measured numbers (expected gap until Green)"
fi

fail "no editor paint list harness (Red expected)"
