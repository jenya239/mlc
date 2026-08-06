#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110f — Green: env-gated overlay metrics HUD.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/frame_overlay_metrics_stable.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF_OVERLAY="$ROOT_DIR/misc/editor/ux/perf_overlay.mlc"
RED="$ROOT_DIR/scripts/run_editor_overlay_metrics_red.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
GLYPH_DAMAGE="$ROOT_DIR/scripts/run_editor_glyph_damage.sh"
REPORT_DIR="${EDITOR_OVERLAY_METRICS_OUT:-$ROOT_DIR/.tmp/editor_overlay_metrics}"
OUT_DIR="${UX_FRAME_OVERLAY_METRICS_OUT:-$ROOT_DIR/tmp/ux_frame_overlay_metrics_stable}"
BIN_OUT="$OUT_DIR/bin"
REPORT_FILE="$REPORT_DIR/report.txt"
L1_LOG="$REPORT_DIR/l1.log"

fail() {
  echo "[editor_overlay_metrics] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing L1 $ENTRY"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF_OVERLAY" ] || fail "missing perf_overlay.mlc"
[ -f "$RED" ] || fail "missing red harness"
[ -f "$WAKE" ] || fail "missing wake"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate"
[ -f "$GLYPH_DAMAGE" ] || fail "missing glyph-damage harness"

grep -q 'editor_perf_overlay_format_lines' "$PERF_OVERLAY" || fail "missing format helper"
grep -q 'MLC_EDITOR_PERF_OVERLAY' "$DEMO" || fail "demo missing OVERLAY env"
grep -q 'editor_perf_overlay_enabled\|editor_perf_overlay_format_lines' "$DEMO" || \
  fail "demo missing overlay wire"
# Overlay helper must not issue widget GL.
if grep -Eq 'solid_renderer_rect|static_text_draw_lines|static_text_glyph_batch_draw|gl_' "$PERF_OVERLAY"; then
  fail "perf_overlay.mlc has GL/widget draw calls"
fi
# Default-off: overlay push only under enabled check.
if ! grep -q 'perf_overlay_on\|editor_perf_overlay_enabled' "$DEMO"; then
  fail "demo missing overlay gate"
fi

rm -rf "$REPORT_DIR" "$OUT_DIR"
mkdir -p "$REPORT_DIR" "$OUT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
# Default path: overlay unset for side gates.
unset MLC_EDITOR_PERF_OVERLAY || true

echo "[editor_overlay_metrics] L1…" >&2
"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
[ -x "$BIN_OUT" ] || fail "missing L1 binary"
"$BIN_OUT" | tee "$L1_LOG"
grep -q 'ux_ok frame_overlay_metrics_stable' "$L1_LOG" || fail "L1 missing ux_ok"
grep -q 'overlay_text_ops=0' "$L1_LOG" || fail "L1 missing default-off text_ops"
grep -q 'overlay_text_ops=4' "$L1_LOG" || fail "L1 missing env-on text_ops"

if bash "$RED" >/tmp/editor_overlay_metrics_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -q 'already present\|already declare' /tmp/editor_overlay_metrics_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/editor_overlay_metrics_red.out))"

echo "[editor_overlay_metrics] compile demo_live…" >&2
bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "[editor_overlay_metrics] wake (overlay unset)…" >&2
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

echo "[editor_overlay_metrics] dogfood gate (overlay unset)…" >&2
DOGFOOD_OUT="$REPORT_DIR/dogfood"
mkdir -p "$DOGFOOD_OUT"
EDITOR_PERF_DOGFOOD_GATE_OUT="$DOGFOOD_OUT" bash "$DOGFOOD_GATE"

echo "[editor_overlay_metrics] glyph-damage (overlay unset)…" >&2
GLYPH_OUT="$REPORT_DIR/glyph_damage"
mkdir -p "$GLYPH_OUT"
EDITOR_GLYPH_DAMAGE_OUT="$GLYPH_OUT" bash "$GLYPH_DAMAGE"

scroll_cpu="$(grep -E 'scroll_cpu_percent=' "$DOGFOOD_OUT/report.txt" 2>/dev/null | tail -1 | sed -n 's/.*scroll_cpu_percent=\([0-9][0-9]*\).*/\1/p' || true)"
if [ -z "$scroll_cpu" ]; then
  scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$GLYPH_OUT/report.txt" 2>/dev/null | cut -d= -f2 || true)"
fi
if [ -n "$scroll_cpu" ] && [ "$scroll_cpu" -gt 50 ]; then
  fail "scroll_cpu_percent=$scroll_cpu > 50 under default (overlay off)"
fi

{
  echo "overlay_env=0"
  echo "overlay_text_ops=0"
  echo "overlay_env=1"
  echo "overlay_text_ops=4"
  echo "delta_layout_generation=0"
  echo "delta_paint_generation=0"
  echo "scroll_cpu_percent=${scroll_cpu:-unknown}"
  echo "l1=ok"
  echo "wake=ok"
  echo "dogfood_gate=ok"
  echo "glyph_damage=ok"
  echo "mechanism=env_gated_paint_list_hud"
} | tee "$REPORT_FILE"

echo "[editor_overlay_metrics] ok" >&2
