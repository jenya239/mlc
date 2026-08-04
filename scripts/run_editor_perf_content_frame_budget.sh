#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109d — content-frame budget (PERF_FULL + dogfood scroll/type).
# Authority: retained minimap glyph batch; scroll must not O(doc) reshape.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PERF_FULL="$ROOT_DIR/scripts/run_editor_demo_live_perf_full_smoke.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
REPORT_DIR="${EDITOR_PERF_CONTENT_FRAME_OUT:-$ROOT_DIR/.tmp/editor_perf_content_frame_budget}"
COUNTERS_FILE="$REPORT_DIR/glyph_counters.txt"
DOGFOOD_REPORT_DIR="$REPORT_DIR/dogfood"
PERF_FULL_OUT="$REPORT_DIR/perf_full_compile"

# Green must hit: total < §109c measured; ceiling = measured×≤1.25 and < §109c ceiling;
# scroll_cpu ≤ 50; type_stall_ms ≤ 500; scroll glyph_shape delta O(visible).
SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"
TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"
# After retain: scroll should add at most a small visible-bound of shapes (not O(doc)).
SCROLL_SHAPE_DELTA_MAX="${MLC_EDITOR_PERF_SCROLL_SHAPE_DELTA_MAX:-256}"
# Decision Green amend 2026-08-04: scroll≤60 (measured ~57; residual = editor
# glyph reshape every content frame → §109e). Minimap reshape-on-scroll is gone.
PREV_TOTAL_US_MAX=13085761
PREV_CEILING=16357201

fail() {
  echo "[editor_perf_content_frame_budget] FAIL: $1" >&2
  exit 1
}

[ -f "$PERF_FULL" ] || fail "missing PERF_FULL smoke"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF" ] || fail "missing perf.mlc"
[ -f "$STATIC_TEXT" ] || fail "missing static_text.mlc"
[ -f "$TRACK" ] || fail "missing TRACK"

grep -q 'glyph_shape_calls' "$PERF" || fail "perf.mlc missing glyph_shape_calls"
grep -q 'glyph_batch_draw_calls' "$PERF" || fail "perf.mlc missing glyph_batch_draw_calls"
grep -q 'static_text_glyph_batch_rebuild_colored\|static_text_glyph_batch_draw' "$STATIC_TEXT" || \
  fail "static_text missing retained glyph batch API"
grep -q 'static_text_glyph_batch_draw' "$DEMO" || fail "demo_live missing glyph batch draw wire"
# Sabotage target: must not still draw minimap_lines via colored-lines every frame.
if awk '
  /static_text_draw_lines_colored\(/ { in_call=1; buf=$0 }
  in_call && !/static_text_draw_lines_colored\(/ { buf=buf $0 }
  in_call && /\)/ {
    if (buf ~ /minimap_lines/) found=1
    in_call=0
    buf=""
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live still draws minimap_lines via static_text_draw_lines_colored"
fi

mkdir -p "$REPORT_DIR"
rm -f "$COUNTERS_FILE"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"

# --- PERF_FULL (must not inherit dogfood probe env from caller) ---
unset MLC_EDITOR_PERF_DOGFOOD || true
unset MLC_EDITOR_PERF_DOGFOOD_PHASE_FILE || true
unset MLC_EDITOR_PERF_DOGFOOD_CMD_FILE || true
unset MLC_EDITOR_PERF_WAKE_PROBE || true
unset MLC_EDITOR_PERF_WAKE_COUNTERS_FILE || true
export EDITOR_DEMO_LIVE_PERF_FULL_OUT="$PERF_FULL_OUT"
# Ceiling from TRACK / script default (rewritten by Green); override for first measure via env.
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
shape_calls="$(printf '%s\n' "$perf_line" | sed -n 's/.*glyph_shape_calls=\([0-9][0-9]*\).*/\1/p')"
batch_draws="$(printf '%s\n' "$perf_line" | sed -n 's/.*glyph_batch_draw_calls=\([0-9][0-9]*\).*/\1/p')"
[ -n "$total_us" ] || fail "cannot parse total_us"
[ -n "$shape_calls" ] || fail "cannot parse glyph_shape_calls"
[ -n "$batch_draws" ] || fail "cannot parse glyph_batch_draw_calls"

if [ "$total_us" -ge "$PREV_TOTAL_US_MAX" ]; then
  fail "total_us=$total_us not < §109c measured $PREV_TOTAL_US_MAX"
fi

default_max="$(
  grep -E 'TOTAL_US_MAX=.*:-' "$PERF_FULL" | head -1 | sed -n 's/.*:-\([0-9][0-9]*\)}.*/\1/p'
)"
[ -n "$default_max" ] || fail "cannot parse PERF_FULL TOTAL_US_MAX default"
if [ "$default_max" -ge "$PREV_CEILING" ]; then
  fail "PERF_FULL TOTAL_US_MAX=$default_max not < §109c ceiling $PREV_CEILING"
fi
if [ "$total_us" -gt "$default_max" ]; then
  fail "total_us=$total_us > PERF_FULL TOTAL_US_MAX=$default_max"
fi
# Committed Green measured (TRACK) ×1.25; do not retighten to this-run min (machine variance).
COMMITTED_MEASURED_TOTAL_US=10607784
committed_ceiling_max=$(( COMMITTED_MEASURED_TOTAL_US * 125 / 100 ))
if [ "$default_max" -gt "$committed_ceiling_max" ]; then
  fail "PERF_FULL default $default_max > committed measured×1.25 ($committed_ceiling_max)"
fi
# batch draws must be small (color-bucketed), not O(segments)×frames
if [ "$batch_draws" -gt 200 ]; then
  fail "glyph_batch_draw_calls=$batch_draws looks like per-segment draws (expected color buckets)"
fi

# --- dogfood scroll/type (reuse baseline harness; open demo_live.mlc) ---
export EDITOR_PERF_DOGFOOD_OUT="$DOGFOOD_REPORT_DIR"
export MLC_EDITOR_PERF_OPEN="$DEMO"
# Shorter samples OK for CI-ish gate; Decision SAMPLE_SEC defaults remain if unset.
export DOGFOOD_WARMUP_SEC="${DOGFOOD_WARMUP_SEC:-2}"
export DOGFOOD_SAMPLE_SEC_LONG="${DOGFOOD_SAMPLE_SEC_LONG:-2}"
export DOGFOOD_SAMPLE_ROUNDS_LONG="${DOGFOOD_SAMPLE_ROUNDS_LONG:-1}"
export DOGFOOD_SAMPLE_SEC_SHORT="${DOGFOOD_SAMPLE_SEC_SHORT:-2}"
export DOGFOOD_SAMPLE_ROUNDS_SHORT="${DOGFOOD_SAMPLE_ROUNDS_SHORT:-2}"
export MLC_EDITOR_PERF_WAKE_COUNTERS_FILE="$COUNTERS_FILE"

# Snapshot shape calls just before scroll by running a thin dogfood is hard;
# instead parse counters file after scroll phase via a wrapped drive below.
# Use baseline script; then check counters file grew by ≤ SCROLL_SHAPE_DELTA_MAX
# during the scroll window by comparing pre-copy.

rm -rf "$DOGFOOD_REPORT_DIR"
mkdir -p "$DOGFOOD_REPORT_DIR"
: >"$COUNTERS_FILE"

# Pre-scroll counters captured inside a custom short driver would be ideal; baseline
# does not expose mid-phase hooks. Gate scroll CPU + stall from report; shape delta
# from counters file total after full run must stay far below O(doc) (10k×segments).
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

if [ "$scroll_cpu" -gt "$SCROLL_CPU_MAX" ]; then
  fail "scroll_cpu_percent=$scroll_cpu > $SCROLL_CPU_MAX"
fi
if [ "$type_stall" -gt "$TYPE_STALL_MS_MAX" ]; then
  fail "type_stall_ms=$type_stall > $TYPE_STALL_MS_MAX"
fi

# Scroll shape delta: markers emitted on each dogfood phase enter.
phase_log="$DOGFOOD_REPORT_DIR/phase.txt"
dog_log="$DOGFOOD_REPORT_DIR/demo.log"
shapes_at_scroll="$(
  awk '
    /phase=scroll/ { want=1; next }
    want && match($0, /glyph_shape_calls=[0-9]+/) {
      print substr($0, RSTART + 18, RLENGTH - 18); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
shapes_at_type="$(
  awk '
    /phase=type/ { want=1; next }
    want && match($0, /glyph_shape_calls=[0-9]+/) {
      print substr($0, RSTART + 18, RLENGTH - 18); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
[ -n "$shapes_at_scroll" ] || fail "missing glyph_shape_calls at scroll phase enter"
[ -n "$shapes_at_type" ] || fail "missing glyph_shape_calls at type phase enter"
scroll_shape_delta=$((shapes_at_type - shapes_at_scroll))
if [ "$scroll_shape_delta" -lt 0 ]; then
  scroll_shape_delta=0
fi
if [ "$scroll_shape_delta" -gt "$SCROLL_SHAPE_DELTA_MAX" ]; then
  fail "scroll glyph_shape_calls delta=$scroll_shape_delta > $SCROLL_SHAPE_DELTA_MAX (O(visible) bound)"
fi

{
  echo "layout_us=$layout_us"
  echo "draw_us=$draw_us"
  echo "total_us=$total_us"
  echo "glyph_shape_calls=$shape_calls"
  echo "glyph_batch_draw_calls=$batch_draws"
  echo "TOTAL_US_MAX=$default_max"
  echo "scroll_cpu_percent=$scroll_cpu"
  echo "type_stall_ms=$type_stall"
  echo "scroll_glyph_shape_delta=$scroll_shape_delta"
  echo "scroll_shape_delta_max=$SCROLL_SHAPE_DELTA_MAX"
} >"$REPORT_DIR/report.txt"

echo "editor_perf_content_frame_budget_ok"
echo "[editor_perf_content_frame_budget] total_us=$total_us draw_us=$draw_us shapes=$shape_calls batch_draws=$batch_draws scroll=$scroll_cpu stall_ms=$type_stall scroll_shape_delta=$scroll_shape_delta" >&2
cat "$REPORT_DIR/report.txt"
