#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110e — Green: glyph Y-adjust / newly-visible.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/frame_glyph_damage_stable.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
GLYPH_DAMAGE="$ROOT_DIR/misc/editor/ux/glyph_damage.mlc"
RED="$ROOT_DIR/scripts/run_editor_glyph_damage_red.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
GLYPH_LAYER="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
REPORT_DIR="${EDITOR_GLYPH_DAMAGE_OUT:-$ROOT_DIR/.tmp/editor_glyph_damage}"
OUT_DIR="${UX_FRAME_GLYPH_DAMAGE_OUT:-$ROOT_DIR/tmp/ux_frame_glyph_damage_stable}"
BIN_OUT="$OUT_DIR/bin"
REPORT_FILE="$REPORT_DIR/report.txt"
L1_LOG="$REPORT_DIR/l1.log"

# Absolute avg shape/frame ceiling when newly_visible not in dogfood delta math.
SCROLL_SHAPE_AVG_MAX="${MLC_EDITOR_GLYPH_DAMAGE_SHAPE_AVG_MAX:-64}"
# Measured-then-written below 60 (Green updates dogfood default).
SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-50}"

fail() {
  echo "[editor_glyph_damage] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing L1 $ENTRY"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$STATIC_TEXT" ] || fail "missing static_text.mlc"
[ -f "$GLYPH_DAMAGE" ] || fail "missing glyph_damage.mlc"
[ -f "$RED" ] || fail "missing red harness"
[ -f "$WAKE" ] || fail "missing wake"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate"
[ -f "$GLYPH_LAYER" ] || fail "missing glyph-layer budget"

grep -q 'static_text_glyph_batch_adjust_y' "$STATIC_TEXT" || fail "missing adjust_y"
grep -q 'static_text_glyph_batch_append_colored\|newly_visible' "$STATIC_TEXT" "$DEMO" || \
  fail "missing append/newly_visible path"
grep -q 'editor_glyph_newly_visible_rows\|newly_visible_rows' "$DEMO" "$GLYPH_DAMAGE" || \
  fail "missing newly_visible_rows wiring"
# Reshape fingerprint must not key scroll_offset_y into full rebuild.
if awk '
  /editor_glyph_fp =$/ || /editor_glyph_fp =/ { in_fp=1 }
  in_fp {
    if ($0 ~ /scroll_offset_y/) found=1
    if ($0 ~ /theme\.keyword_red/ || $0 ~ /editor_lines\.length/) in_fp=0
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live editor_glyph_fp still includes scroll_offset_y"
fi
if ! grep -q 'SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-50}"' "$DOGFOOD_GATE"; then
  fail "dogfood SCROLL_CPU_MAX default is not 50 (measured §110e ceiling)"
fi
default_scroll=50

rm -rf "$REPORT_DIR" "$OUT_DIR"
mkdir -p "$REPORT_DIR" "$OUT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

echo "[editor_glyph_damage] L1…" >&2
"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
[ -x "$BIN_OUT" ] || fail "missing L1 binary"
"$BIN_OUT" | tee "$L1_LOG"
grep -q 'ux_ok frame_glyph_damage_stable' "$L1_LOG" || fail "L1 missing ux_ok"
grep -q 'newly_visible_rows=' "$L1_LOG" || fail "L1 missing newly_visible_rows"
grep -q 'scroll_shape_bound=' "$L1_LOG" || fail "L1 missing scroll_shape_bound"

if bash "$RED" >/tmp/editor_glyph_damage_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -q 'already present\|already declare\|default is not 60' /tmp/editor_glyph_damage_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/editor_glyph_damage_red.out))"

echo "[editor_glyph_damage] compile demo_live…" >&2
bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "[editor_glyph_damage] glyph-layer budget (shape avg)…" >&2
GLYPH_OUT="$REPORT_DIR/glyph_layer"
mkdir -p "$GLYPH_OUT"
# Use tightened scroll ceiling for nested dogfood inside glyph-layer.
export MLC_EDITOR_PERF_SCROLL_CPU_MAX="$default_scroll"
export MLC_EDITOR_PERF_VISIBLE_ROW_BUDGET="${MLC_EDITOR_PERF_VISIBLE_ROW_BUDGET:-48}"
# Override §109e avg max with §110e absolute ≤64 via env if glyph-layer supports it —
# glyph-layer computes 4*budget+64; we re-check avg from its report against 64.
EDITOR_PERF_GLYPH_LAYER_OUT="$GLYPH_OUT" bash "$GLYPH_LAYER"
shape_avg="$(grep -E '^scroll_editor_glyph_shape_avg=' "$GLYPH_OUT/report.txt" | cut -d= -f2 || true)"
[ -n "$shape_avg" ] || fail "missing scroll_editor_glyph_shape_avg"
if [ "$shape_avg" -gt "$SCROLL_SHAPE_AVG_MAX" ]; then
  fail "scroll shape avg=$shape_avg > $SCROLL_SHAPE_AVG_MAX"
fi

echo "[editor_glyph_damage] wake…" >&2
WAKE_OUT="$REPORT_DIR/wake"
mkdir -p "$WAKE_OUT"
EDITOR_PERF_WAKE_OUT="$WAKE_OUT" bash "$WAKE"
layout_b=$(grep -E '^layout_generation=' "$WAKE_OUT/counters_still_before.txt" | tail -1 | cut -d= -f2)
layout_a=$(grep -E '^layout_generation=' "$WAKE_OUT/counters_still_after.txt" | tail -1 | cut -d= -f2)
paint_b=$(grep -E '^paint_generation=' "$WAKE_OUT/counters_still_before.txt" | tail -1 | cut -d= -f2)
paint_a=$(grep -E '^paint_generation=' "$WAKE_OUT/counters_still_after.txt" | tail -1 | cut -d= -f2)
[ -n "$layout_b" ] && [ -n "$layout_a" ] || fail "missing layout_generation"
[ -n "$paint_b" ] && [ -n "$paint_a" ] || fail "missing paint_generation"
if [ "$((layout_a - layout_b))" -ne 0 ] || [ "$((paint_a - paint_b))" -ne 0 ]; then
  fail "ownership gens moved during still (layout $layout_b->$layout_a paint $paint_b->$paint_a)"
fi

echo "[editor_glyph_damage] dogfood gate…" >&2
DOGFOOD_OUT="$REPORT_DIR/dogfood"
mkdir -p "$DOGFOOD_OUT"
EDITOR_PERF_DOGFOOD_GATE_OUT="$DOGFOOD_OUT" bash "$DOGFOOD_GATE"

scroll_cpu="$(grep -E 'scroll_cpu_percent=' "$DOGFOOD_OUT/report.txt" 2>/dev/null | tail -1 | sed -n 's/.*scroll_cpu_percent=\([0-9][0-9]*\).*/\1/p' || true)"
if [ -z "$scroll_cpu" ]; then
  scroll_cpu="$(grep -E 'scroll_cpu' "$GLYPH_OUT/report.txt" 2>/dev/null | head -1 | sed -n 's/.*scroll_cpu_percent=\([0-9][0-9]*\).*/\1/p' || true)"
fi
if [ -z "$scroll_cpu" ]; then
  scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$GLYPH_OUT/report.txt" | cut -d= -f2 || true)"
fi

{
  echo "newly_visible_rows=$(grep -E '^newly_visible_rows=' "$L1_LOG" | cut -d= -f2)"
  echo "scroll_shape_bound=$(grep -E '^scroll_shape_bound=' "$L1_LOG" | cut -d= -f2)"
  echo "scroll_editor_glyph_shape_avg=$shape_avg"
  echo "scroll_shape_avg_max=$SCROLL_SHAPE_AVG_MAX"
  echo "SCROLL_CPU_MAX=$default_scroll"
  echo "scroll_cpu_percent=${scroll_cpu:-unknown}"
  echo "delta_layout_generation=0"
  echo "delta_paint_generation=0"
  echo "l1=ok"
  echo "wake=ok"
  echo "dogfood_gate=ok"
  echo "mechanism=adjust_y_plus_newly_visible_append"
} | tee "$REPORT_FILE"

echo "[editor_glyph_damage] ok" >&2
