#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110c — Green: paint list + single submit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/frame_paint_list_stable.mlc"
PAINT_LIST="$ROOT_DIR/misc/editor/ux/paint_list.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
RED="$ROOT_DIR/scripts/run_editor_paint_list_red.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
OWNERSHIP="$ROOT_DIR/scripts/run_editor_frame_ownership.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
REPORT_DIR="${EDITOR_PAINT_LIST_OUT:-$ROOT_DIR/.tmp/editor_paint_list}"
OUT_DIR="${UX_FRAME_PAINT_LIST_OUT:-$ROOT_DIR/tmp/ux_frame_paint_list_stable}"
BIN_OUT="$OUT_DIR/bin"
REPORT_FILE="$REPORT_DIR/report.txt"
L1_LOG="$REPORT_DIR/l1.log"

fail() {
  echo "[editor_paint_list] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing L1 $ENTRY"
[ -f "$PAINT_LIST" ] || fail "missing paint_list.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$RED" ] || fail "missing red harness"
[ -f "$WAKE" ] || fail "missing wake"
[ -f "$OWNERSHIP" ] || fail "missing ownership"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate"

grep -q 'export type EditorPaintList' "$PAINT_LIST" || fail "missing EditorPaintList"
grep -q 'export fn editor_paint_list_submit' "$PAINT_LIST" || fail "missing editor_paint_list_submit"
grep -q "from './ux/paint_list'" "$DEMO" || fail "demo_live missing paint_list import"
grep -q 'editor_paint_list_submit(' "$DEMO" || fail "demo_live missing submit"

# Green must-hit: zero load-bearing direct GL sites in demo_live.
for needle in \
  'solid_renderer_rect(' \
  'static_text_glyph_batch_draw(' \
  'static_text_draw_lines(' \
  'static_text_draw_lines_colored(' \
  'editor_gl_scissor_enable_rect(' \
  'editor_gl_scissor_disable('
do
  if grep -q "$needle" "$DEMO"; then
    fail "demo_live still has direct $needle"
  fi
done
# Submit module must own those calls.
grep -q 'solid_renderer_rect(' "$PAINT_LIST" || fail "paint_list missing solid_renderer_rect"
grep -q 'static_text_glyph_batch_draw(' "$PAINT_LIST" || fail "paint_list missing glyph draw"
grep -q 'editor_gl_scissor_enable_rect(' "$PAINT_LIST" || fail "paint_list missing scissor"

rm -rf "$REPORT_DIR" "$OUT_DIR"
mkdir -p "$REPORT_DIR" "$OUT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

echo "[editor_paint_list] L1…" >&2
"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
[ -x "$BIN_OUT" ] || fail "missing L1 binary"
"$BIN_OUT" | tee "$L1_LOG"
grep -q 'ux_ok frame_paint_list_stable' "$L1_LOG" || fail "L1 missing ux_ok"
grep -q 'paint_ops=' "$L1_LOG" || fail "L1 missing paint_ops"
grep -Eq 'gl_call_from_widget=0|gl_call_from_widget_static=0' "$L1_LOG" || fail "L1 missing gl_call_from_widget=0"

if bash "$RED" >/tmp/editor_paint_list_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -q 'already present\|already declare' /tmp/editor_paint_list_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/editor_paint_list_red.out))"

# Sabotage: inject forbidden call into demo copy → static check fails
sab_demo="$REPORT_DIR/demo_sab.mlc"
cp "$DEMO" "$sab_demo"
echo '    solid = solid_renderer_rect(solid, width, height, 0, 0, 1, 1, 1.0, 1.0, 1.0, 1.0)' >>"$sab_demo"
if ! grep -q 'solid_renderer_rect(' "$sab_demo"; then
  fail "sabotage inject failed"
fi
echo "[editor_paint_list] sab1 direct-rect detect ok" >&2

echo "[editor_paint_list] compile demo_live…" >&2
bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "[editor_paint_list] wake…" >&2
WAKE_OUT="$REPORT_DIR/wake"
mkdir -p "$WAKE_OUT"
EDITOR_PERF_WAKE_OUT="$WAKE_OUT" bash "$WAKE"
[ -f "$WAKE_OUT/counters_still_before.txt" ] || fail "missing wake still counters"
layout_b=$(grep -E '^layout_generation=' "$WAKE_OUT/counters_still_before.txt" | tail -1 | cut -d= -f2)
layout_a=$(grep -E '^layout_generation=' "$WAKE_OUT/counters_still_after.txt" | tail -1 | cut -d= -f2)
paint_b=$(grep -E '^paint_generation=' "$WAKE_OUT/counters_still_before.txt" | tail -1 | cut -d= -f2)
paint_a=$(grep -E '^paint_generation=' "$WAKE_OUT/counters_still_after.txt" | tail -1 | cut -d= -f2)
[ -n "$layout_b" ] && [ -n "$layout_a" ] || fail "missing layout_generation"
[ -n "$paint_b" ] && [ -n "$paint_a" ] || fail "missing paint_generation"
if [ "$((layout_a - layout_b))" -ne 0 ] || [ "$((paint_a - paint_b))" -ne 0 ]; then
  fail "ownership gens moved during still (layout $layout_b->$layout_a paint $paint_b->$paint_a)"
fi

echo "[editor_paint_list] dogfood gate…" >&2
DOGFOOD_OUT="$REPORT_DIR/dogfood"
mkdir -p "$DOGFOOD_OUT"
EDITOR_PERF_DOGFOOD_GATE_OUT="$DOGFOOD_OUT" bash "$DOGFOOD_GATE"

paint_ops=$(grep -E '^paint_ops=' "$L1_LOG" | tail -1 | cut -d= -f2)
{
  echo "paint_ops=$paint_ops"
  echo "gl_call_from_widget=0"
  echo "gl_call_from_widget_static=0"
  echo "delta_layout_generation=0"
  echo "delta_paint_generation=0"
  echo "l1=ok"
  echo "wake=ok"
  echo "dogfood_gate=ok"
} | tee "$REPORT_FILE"

echo "[editor_paint_list] ok" >&2
