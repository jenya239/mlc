#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110d — Green: batch/stream solid upload.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/frame_batch_stream_stable.mlc"
SOLID="$ROOT_DIR/misc/gui/solid_renderer.mlc"
PAINT_LIST="$ROOT_DIR/misc/editor/ux/paint_list.mlc"
PAINT_OPS="$ROOT_DIR/misc/editor/ux/paint_ops.mlc"
TERMINAL_GRID="$ROOT_DIR/misc/editor/terminal/terminal_grid_render.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
RED="$ROOT_DIR/scripts/run_editor_batch_stream_red.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
REPORT_DIR="${EDITOR_BATCH_STREAM_OUT:-$ROOT_DIR/.tmp/editor_batch_stream}"
OUT_DIR="${UX_FRAME_BATCH_STREAM_OUT:-$ROOT_DIR/tmp/ux_frame_batch_stream_stable}"
BIN_OUT="$OUT_DIR/bin"
REPORT_FILE="$REPORT_DIR/report.txt"
L1_LOG="$REPORT_DIR/l1.log"

fail() {
  echo "[editor_batch_stream] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing L1 $ENTRY"
[ -f "$SOLID" ] || fail "missing solid_renderer.mlc"
[ -f "$PAINT_LIST" ] || fail "missing paint_list.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$RED" ] || fail "missing red harness"
[ -f "$WAKE" ] || fail "missing wake"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate"

grep -q 'solid_upload_bytes' "$SOLID" || fail "missing solid_upload_bytes"
grep -q 'solid_stream_upload_bytes_for_flush' "$SOLID" || fail "missing upload helper"
grep -q 'buffer_data_orphan\|gl_buffer_data_orphan_scratch' "$SOLID" || fail "missing orphan upload path"
grep -q 'editor_paint_ops_coalesce_adjacent' "$PAINT_OPS" || fail "missing coalesce"
grep -q 'terminal_grid_background_paint_ops' "$TERMINAL_GRID" || fail "missing terminal emit"
grep -q 'terminal_grid_background_paint_ops' "$DEMO" || fail "demo missing terminal emit"
if grep -q 'terminal_grid_draw_cached_backgrounds' "$DEMO"; then
  fail "demo_live still calls terminal_grid_draw_cached_backgrounds"
fi
if grep -q 'solid_renderer_rect(' "$DEMO"; then
  fail "demo_live still has solid_renderer_rect"
fi

rm -rf "$REPORT_DIR" "$OUT_DIR"
mkdir -p "$REPORT_DIR" "$OUT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

echo "[editor_batch_stream] L1…" >&2
"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
[ -x "$BIN_OUT" ] || fail "missing L1 binary"
"$BIN_OUT" | tee "$L1_LOG"
grep -q 'ux_ok frame_batch_stream_stable' "$L1_LOG" || fail "L1 missing ux_ok"
grep -q 'solid_upload_bytes=' "$L1_LOG" || fail "L1 missing solid_upload_bytes"
grep -q 'idle_solid_upload_bytes=0' "$L1_LOG" || fail "L1 idle upload not 0"
grep -q 'draw_calls=' "$L1_LOG" || fail "L1 missing draw_calls"

if bash "$RED" >/tmp/editor_batch_stream_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -q 'already present\|already declare' /tmp/editor_batch_stream_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/editor_batch_stream_red.out))"

echo "[editor_batch_stream] compile demo_live…" >&2
bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "[editor_batch_stream] wake…" >&2
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

echo "[editor_batch_stream] dogfood gate…" >&2
DOGFOOD_OUT="$REPORT_DIR/dogfood"
mkdir -p "$DOGFOOD_OUT"
EDITOR_PERF_DOGFOOD_GATE_OUT="$DOGFOOD_OUT" bash "$DOGFOOD_GATE"

upload=$(grep -E '^solid_upload_bytes=' "$L1_LOG" | tail -1 | cut -d= -f2)
idle=$(grep -E '^idle_solid_upload_bytes=' "$L1_LOG" | tail -1 | cut -d= -f2)
draw_calls=$(grep -E '^draw_calls=' "$L1_LOG" | tail -1 | cut -d= -f2)
rects_before=$(grep -E '^rects_before=' "$L1_LOG" | tail -1 | cut -d= -f2)
rects_after=$(grep -E '^rects_after=' "$L1_LOG" | tail -1 | cut -d= -f2)
{
  echo "solid_flush_count=1"
  echo "solid_upload_bytes=$upload"
  echo "idle_solid_upload_bytes=$idle"
  echo "draw_calls=$draw_calls"
  echo "rects_before=$rects_before"
  echo "rects_after=$rects_after"
  echo "mechanism=orphan_buffer_data"
  echo "delta_layout_generation=0"
  echo "delta_paint_generation=0"
  echo "l1=ok"
  echo "wake=ok"
  echo "dogfood_gate=ok"
} | tee "$REPORT_FILE"

echo "[editor_batch_stream] ok" >&2
