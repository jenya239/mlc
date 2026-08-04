#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109e — editor retained glyph batch budget.
# Authority: dogfood scroll/type + PERF_FULL; VISIBLE=1; open demo_live.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PERF_FULL="$ROOT_DIR/scripts/run_editor_demo_live_perf_full_smoke.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
REPORT_DIR="${EDITOR_PERF_GLYPH_LAYER_OUT:-$ROOT_DIR/.tmp/editor_perf_glyph_layer_budget}"
DOGFOOD_REPORT_DIR="$REPORT_DIR/dogfood"
PERF_FULL_OUT="$REPORT_DIR/perf_full_compile"

SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"
TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"
# Decision: avg editor shapes/content-frame ≤ 4*visible_row_budget+64.
# Window 1100×760 → content ≈630px / ~18px ≈ 35+3 → budget 38; harness default 48.
VISIBLE_ROW_BUDGET="${MLC_EDITOR_PERF_VISIBLE_ROW_BUDGET:-48}"
SCROLL_SHAPE_AVG_MAX=$((4 * VISIBLE_ROW_BUDGET + 64))
# §109d measured basis; §109e Green may rewrite ceiling (measured×≤1.25 < 13259730).
PREV_TOTAL_US_MAX=10607784
PREV_CEILING=13259730
# Decision amend 2026-08-04: scroll≤60 (measured ~49–63; residual = full visible
# editor VBO reshape each scroll frame without row-level Y-damage).
COMMITTED_MEASURED_TOTAL_US=6565154

fail() {
  echo "[editor_perf_glyph_layer_budget] FAIL: $1" >&2
  exit 1
}

[ -f "$PERF_FULL" ] || fail "missing PERF_FULL smoke"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF" ] || fail "missing perf.mlc"
[ -f "$STATIC_TEXT" ] || fail "missing static_text.mlc"
[ -f "$TRACK" ] || fail "missing TRACK"

grep -q 'editor_glyph_shape_calls' "$PERF" || fail "perf.mlc missing editor_glyph_shape_calls"
grep -q 'editor_glyph_batch' "$DEMO" || fail "demo_live missing editor_glyph_batch"
grep -q 'static_text_glyph_batch_rebuild_colored\|static_text_glyph_batch_draw' "$STATIC_TEXT" || \
  fail "static_text missing retained glyph batch API"
# Must not draw editor_lines via colored-lines every content frame.
if awk '
  /static_text_draw_lines_colored\(/ { in_call=1; buf=$0 }
  in_call && !/static_text_draw_lines_colored\(/ { buf=buf $0 }
  in_call && /\)/ {
    if (buf ~ /editor_lines/) found=1
    in_call=0
    buf=""
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live still draws editor_lines via static_text_draw_lines_colored"
fi

mkdir -p "$REPORT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"

unset MLC_EDITOR_PERF_DOGFOOD || true
unset MLC_EDITOR_PERF_DOGFOOD_PHASE_FILE || true
unset MLC_EDITOR_PERF_DOGFOOD_CMD_FILE || true
unset MLC_EDITOR_PERF_WAKE_PROBE || true
unset MLC_EDITOR_PERF_WAKE_COUNTERS_FILE || true
export EDITOR_DEMO_LIVE_PERF_FULL_OUT="$PERF_FULL_OUT"

set +e
perf_output="$(bash "$PERF_FULL" 2>&1)"
perf_status=$?
set -e
printf '%s\n' "$perf_output" | tee "$REPORT_DIR/perf_full.log"
[ "$perf_status" -eq 0 ] || fail "PERF_FULL smoke failed (exit=$perf_status)"

perf_line="$(printf '%s\n' "$perf_output" | grep -E '\[mlc-editor\] demo_live_perf_full frames=' | tail -n 1 || true)"
[ -n "$perf_line" ] || fail "missing demo_live_perf_full line"
total_us="$(printf '%s\n' "$perf_line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
draw_us="$(printf '%s\n' "$perf_line" | sed -n 's/.*draw_us=\([0-9][0-9]*\).*/\1/p')"
layout_us="$(printf '%s\n' "$perf_line" | sed -n 's/.*layout_us=\([0-9][0-9]*\).*/\1/p')"
editor_shapes="$(printf '%s\n' "$perf_line" | sed -n 's/.*editor_glyph_shape_calls=\([0-9][0-9]*\).*/\1/p')"
[ -n "$total_us" ] || fail "cannot parse total_us"
[ -n "$editor_shapes" ] || fail "cannot parse editor_glyph_shape_calls from PERF_FULL line"

default_max="$(
  grep -E 'TOTAL_US_MAX=.*:-' "$PERF_FULL" | head -1 | sed -n 's/.*:-\([0-9][0-9]*\)}.*/\1/p'
)"
[ -n "$default_max" ] || fail "cannot parse PERF_FULL TOTAL_US_MAX default"

if [ "$total_us" -ge "$PREV_TOTAL_US_MAX" ]; then
  # Allow rewrite path: measured may set new ceiling below PREV_CEILING.
  if [ "$default_max" -ge "$PREV_CEILING" ]; then
    fail "total_us=$total_us not < §109d measured $PREV_TOTAL_US_MAX and ceiling not rewritten"
  fi
fi
if [ "$total_us" -gt "$default_max" ]; then
  fail "total_us=$total_us > PERF_FULL TOTAL_US_MAX=$default_max"
fi
if [ "$default_max" -ge "$PREV_CEILING" ]; then
  fail "PERF_FULL TOTAL_US_MAX=$default_max not < §109d ceiling $PREV_CEILING"
fi
committed_ceiling_max=$(( COMMITTED_MEASURED_TOTAL_US * 125 / 100 ))
if [ "$default_max" -gt "$committed_ceiling_max" ]; then
  fail "PERF_FULL default $default_max > committed measured×1.25 ($committed_ceiling_max)"
fi

export EDITOR_PERF_DOGFOOD_OUT="$DOGFOOD_REPORT_DIR"
export MLC_EDITOR_PERF_OPEN="$DEMO"
export DOGFOOD_WARMUP_SEC="${DOGFOOD_WARMUP_SEC:-2}"
export DOGFOOD_SAMPLE_SEC_LONG="${DOGFOOD_SAMPLE_SEC_LONG:-2}"
export DOGFOOD_SAMPLE_ROUNDS_LONG="${DOGFOOD_SAMPLE_ROUNDS_LONG:-1}"
export DOGFOOD_SAMPLE_SEC_SHORT="${DOGFOOD_SAMPLE_SEC_SHORT:-2}"
export DOGFOOD_SAMPLE_ROUNDS_SHORT="${DOGFOOD_SAMPLE_ROUNDS_SHORT:-2}"

rm -rf "$DOGFOOD_REPORT_DIR"
mkdir -p "$DOGFOOD_REPORT_DIR"

set +e
dog_output="$(bash "$DOGFOOD" 2>&1)"
dog_status=$?
set -e
printf '%s\n' "$dog_output" | tee "$REPORT_DIR/dogfood.log"
[ "$dog_status" -eq 0 ] || fail "dogfood baseline failed (exit=$dog_status)"

report="$DOGFOOD_REPORT_DIR/report.txt"
[ -f "$report" ] || fail "missing dogfood report"
scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$report" | cut -d= -f2)"
type_stall="$(grep -E '^type_stall_ms=' "$report" | cut -d= -f2)"
[ -n "$scroll_cpu" ] || fail "missing scroll_cpu_percent"
[ -n "$type_stall" ] || fail "missing type_stall_ms"

# Host /proc CPU% for scroll is noisy (observed 47–64 on same binary). Take median
# of 3 dogfood scroll samples when the first exceeds SCROLL_CPU_MAX.
if [ "$scroll_cpu" -gt "$SCROLL_CPU_MAX" ]; then
  scroll_samples="$scroll_cpu"
  for _retry in 1 2; do
    retry_dir="$REPORT_DIR/dogfood_retry_${_retry}"
    export EDITOR_PERF_DOGFOOD_OUT="$retry_dir"
    rm -rf "$retry_dir"
    mkdir -p "$retry_dir"
    set +e
    bash "$DOGFOOD" >/dev/null 2>"$REPORT_DIR/dogfood_retry_${_retry}.log"
    retry_status=$?
    set -e
    [ "$retry_status" -eq 0 ] || fail "dogfood scroll retry $_retry failed"
    retry_scroll="$(grep -E '^scroll_cpu_percent=' "$retry_dir/report.txt" | cut -d= -f2)"
    [ -n "$retry_scroll" ] || fail "missing scroll_cpu on retry $_retry"
    scroll_samples="$scroll_samples $retry_scroll"
  done
  scroll_cpu="$(
    printf '%s\n' $scroll_samples | sort -n | awk 'NR==2 { print; exit }'
  )"
  echo "[editor_perf_glyph_layer_budget] scroll samples: $scroll_samples → median=$scroll_cpu" >&2
fi

if [ "$scroll_cpu" -gt "$SCROLL_CPU_MAX" ]; then
  fail "scroll_cpu_percent=$scroll_cpu > $SCROLL_CPU_MAX"
fi
if [ "$type_stall" -gt "$TYPE_STALL_MS_MAX" ]; then
  fail "type_stall_ms=$type_stall > $TYPE_STALL_MS_MAX"
fi

phase_log="$DOGFOOD_REPORT_DIR/phase.txt"
dog_log="$DOGFOOD_REPORT_DIR/demo.log"
shapes_at_still="$(
  awk '
    /phase=still_over_text/ { want=1; next }
    want && match($0, /editor_glyph_shape_calls=[0-9]+/) {
      print substr($0, RSTART + 25, RLENGTH - 25); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
shapes_at_scroll="$(
  awk '
    /phase=scroll/ { want=1; next }
    want && match($0, /editor_glyph_shape_calls=[0-9]+/) {
      print substr($0, RSTART + 25, RLENGTH - 25); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
frames_at_scroll="$(
  awk '
    /phase=scroll/ { want=1; next }
    want && match($0, /content_frame_count=[0-9]+/) {
      print substr($0, RSTART + 20, RLENGTH - 20); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
shapes_at_type="$(
  awk '
    /phase=type/ { want=1; next }
    want && match($0, /editor_glyph_shape_calls=[0-9]+/) {
      print substr($0, RSTART + 25, RLENGTH - 25); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
frames_at_type="$(
  awk '
    /phase=type/ { want=1; next }
    want && match($0, /content_frame_count=[0-9]+/) {
      print substr($0, RSTART + 20, RLENGTH - 20); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
[ -n "$shapes_at_still" ] || fail "missing editor_glyph_shape_calls at still_over_text phase enter"
[ -n "$shapes_at_scroll" ] || fail "missing editor_glyph_shape_calls at scroll phase enter"
[ -n "$shapes_at_type" ] || fail "missing editor_glyph_shape_calls at type phase enter"
[ -n "$frames_at_scroll" ] || fail "missing content_frame_count at scroll phase enter"
[ -n "$frames_at_type" ] || fail "missing content_frame_count at type phase enter"

# Decision settle (4): still→scroll without doc edit must not add editor shapes
# (fingerprint retain). Sabotage: clear fingerprint every paint + force content_dirty
# on still → shapes_at_scroll > shapes_at_still.
if [ "$shapes_at_scroll" -ne "$shapes_at_still" ]; then
  fail "settle broken: editor_glyph_shape_calls still=$shapes_at_still scroll=$shapes_at_scroll (expected equal)"
fi

scroll_shape_delta=$((shapes_at_type - shapes_at_scroll))
scroll_frame_delta=$((frames_at_type - frames_at_scroll))
if [ "$scroll_shape_delta" -lt 0 ]; then
  scroll_shape_delta=0
fi
if [ "$scroll_frame_delta" -lt 1 ]; then
  scroll_frame_delta=1
fi
scroll_shape_avg=$((scroll_shape_delta / scroll_frame_delta))
if [ "$scroll_shape_avg" -gt "$SCROLL_SHAPE_AVG_MAX" ]; then
  fail "scroll editor_glyph_shape_calls avg/frame=$scroll_shape_avg > $SCROLL_SHAPE_AVG_MAX (4*budget+64, budget=$VISIBLE_ROW_BUDGET)"
fi

# Settle checked above: still→scroll editor_glyph_shape_calls equal (fingerprint retain).

{
  echo "layout_us=$layout_us"
  echo "draw_us=$draw_us"
  echo "total_us=$total_us"
  echo "editor_glyph_shape_calls=$editor_shapes"
  echo "TOTAL_US_MAX=$default_max"
  echo "scroll_cpu_percent=$scroll_cpu"
  echo "type_stall_ms=$type_stall"
  echo "scroll_editor_glyph_shape_delta=$scroll_shape_delta"
  echo "scroll_content_frame_delta=$scroll_frame_delta"
  echo "scroll_editor_glyph_shape_avg=$scroll_shape_avg"
  echo "scroll_shape_avg_max=$SCROLL_SHAPE_AVG_MAX"
  echo "visible_row_budget=$VISIBLE_ROW_BUDGET"
  echo "settle_shapes_still=$shapes_at_still"
  echo "settle_shapes_scroll=$shapes_at_scroll"
} >"$REPORT_DIR/report.txt"

echo "[editor_perf_glyph_layer_budget] OK total_us=$total_us scroll_cpu=$scroll_cpu shape_avg=$scroll_shape_avg (max $SCROLL_SHAPE_AVG_MAX) stall_ms=$type_stall settle=$shapes_at_still" >&2
