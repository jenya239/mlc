#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109f — Green: visible-range spans + cover/reuse.
# Authority: dogfood type/scroll counters + §109e glyph-layer non-regress.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
SPAN_CACHE="$ROOT_DIR/misc/editor/ux/syntax_span_cache.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
REPORT_DIR="${EDITOR_PERF_SPANS_VISIBLE_OUT:-$ROOT_DIR/.tmp/editor_perf_spans_visible_only}"
DOGFOOD_REPORT_DIR="$REPORT_DIR/dogfood"

TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"
VISIBLE_ROW_BUDGET="${MLC_EDITOR_PERF_VISIBLE_ROW_BUDGET:-48}"
# Decision: avg span_lex_bytes/content-frame ≤ 8*budget*max_line_bytes+4096.
MAX_LINE_BYTES_ESTIMATE="${MLC_EDITOR_PERF_MAX_LINE_BYTES:-256}"
TYPE_SPAN_LEX_AVG_MAX=$((8 * VISIBLE_ROW_BUDGET * MAX_LINE_BYTES_ESTIMATE + 4096))
SCROLL_REBUILD_DELTA_MAX="${MLC_EDITOR_PERF_SCROLL_SPAN_REBUILD_MAX:-2}"

fail() {
  echo "[editor_perf_spans_visible_only] FAIL: $1" >&2
  exit 1
}

[ -f "$GLYPH" ] || fail "missing glyph-layer budget (§109e required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$SPAN_CACHE" ] || fail "missing syntax_span_cache.mlc"
[ -f "$TRACK" ] || fail "missing TRACK"

grep -q 'span_lex_bytes' "$DEMO" || fail "demo_live missing span_lex_bytes"
grep -q 'range_start <= range_start\|state.range_start <= range_start' "$SPAN_CACHE" || \
  fail "syntax_span_cache missing cover/reuse"

# Must not tick full buffer 0..byte_size() on the live span path.
if awk '
  /frame_layout_tick_spans\(/ { in_call=1; buf=$0 }
  in_call && !/frame_layout_tick_spans\(/ { buf=buf $0 }
  in_call && /\)/ {
    if (buf ~ /0,/ && buf ~ /byte_size\(\)/) found=1
    in_call=0
    buf=""
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live still ticks spans with 0..byte_size()"
fi

mkdir -p "$REPORT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"

# §109e non-regress (scroll_cpu≤60, settle, PERF_FULL ceiling).
export EDITOR_PERF_GLYPH_LAYER_OUT="$REPORT_DIR/glyph_layer"
set +e
glyph_output="$(bash "$GLYPH" 2>&1)"
glyph_status=$?
set -e
printf '%s\n' "$glyph_output" | tee "$REPORT_DIR/glyph_layer.log"
[ "$glyph_status" -eq 0 ] || fail "glyph-layer budget failed (exit=$glyph_status)"

# Dedicated dogfood pass for span counters (glyph already ran dogfood; remeasure
# with markers fresh).
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
type_stall="$(grep -E '^type_stall_ms=' "$report" | cut -d= -f2)"
scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$report" | cut -d= -f2)"
[ -n "$type_stall" ] || fail "missing type_stall_ms"
[ -n "$scroll_cpu" ] || fail "missing scroll_cpu_percent"

if [ "$type_stall" -gt "$TYPE_STALL_MS_MAX" ]; then
  fail "type_stall_ms=$type_stall > $TYPE_STALL_MS_MAX"
fi

phase_log="$DOGFOOD_REPORT_DIR/phase.txt"
dog_log="$DOGFOOD_REPORT_DIR/demo.log"

parse_after_phase() {
  local phase="$1"
  local key="$2"
  awk -v phase="$phase" -v key="$key" '
    $0 ~ ("phase=" phase) { want=1; next }
    want && match($0, key "=[0-9]+") {
      print substr($0, RSTART + length(key) + 1, RLENGTH - length(key) - 1)
      exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
}

lex_at_scroll="$(parse_after_phase scroll span_lex_bytes)"
rebuild_at_scroll="$(parse_after_phase scroll span_rebuild_count)"
frames_at_scroll="$(parse_after_phase scroll content_frame_count)"
lex_at_type="$(parse_after_phase type span_lex_bytes)"
rebuild_at_type="$(parse_after_phase type span_rebuild_count)"
frames_at_type="$(parse_after_phase type content_frame_count)"

# type_burst_done markers (after 20 keys) — end of type lex window.
lex_at_burst="$(
  awk '
    /type_burst_done/ { want=1; next }
    want && match($0, /span_lex_bytes=[0-9]+/) {
      print substr($0, RSTART + 15, RLENGTH - 15); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"
frames_at_burst="$(
  awk '
    /type_burst_done/ { want=1; next }
    want && match($0, /content_frame_count=[0-9]+/) {
      print substr($0, RSTART + 20, RLENGTH - 20); exit
    }
  ' "$phase_log" "$dog_log" 2>/dev/null || true
)"

[ -n "$rebuild_at_scroll" ] || fail "missing span_rebuild_count at scroll"
[ -n "$rebuild_at_type" ] || fail "missing span_rebuild_count at type"
[ -n "$lex_at_type" ] || fail "missing span_lex_bytes at type"
[ -n "$lex_at_burst" ] || fail "missing span_lex_bytes at type_burst_done"
[ -n "$frames_at_type" ] || fail "missing content_frame_count at type"
[ -n "$frames_at_burst" ] || fail "missing content_frame_count at type_burst_done"

scroll_rebuild_delta=$((rebuild_at_type - rebuild_at_scroll))
if [ "$scroll_rebuild_delta" -lt 0 ]; then
  fail "span_rebuild_count decreased scroll→type ($rebuild_at_scroll → $rebuild_at_type)"
fi
if [ "$scroll_rebuild_delta" -gt "$SCROLL_REBUILD_DELTA_MAX" ]; then
  fail "scroll span_rebuild_count delta=$scroll_rebuild_delta > $SCROLL_REBUILD_DELTA_MAX (scroll=$rebuild_at_scroll type=$rebuild_at_type)"
fi

type_lex_delta=$((lex_at_burst - lex_at_type))
type_frames_delta=$((frames_at_burst - frames_at_type))
if [ "$type_lex_delta" -lt 0 ]; then
  fail "span_lex_bytes decreased type→burst"
fi
if [ "$type_frames_delta" -lt 1 ]; then
  fail "content_frame_count delta type→burst < 1 (type=$frames_at_type burst=$frames_at_burst)"
fi
type_lex_avg=$((type_lex_delta / type_frames_delta))
if [ "$type_lex_avg" -gt "$TYPE_SPAN_LEX_AVG_MAX" ]; then
  fail "type span_lex_bytes avg=$type_lex_avg > $TYPE_SPAN_LEX_AVG_MAX (delta_lex=$type_lex_delta frames=$type_frames_delta)"
fi

{
  echo "type_stall_ms=$type_stall"
  echo "scroll_cpu_percent=$scroll_cpu"
  echo "scroll_span_rebuild_delta=$scroll_rebuild_delta"
  echo "type_span_lex_bytes_avg=$type_lex_avg"
  echo "type_span_lex_avg_max=$TYPE_SPAN_LEX_AVG_MAX"
  echo "visible_row_budget=$VISIBLE_ROW_BUDGET"
  echo "max_line_bytes_estimate=$MAX_LINE_BYTES_ESTIMATE"
  echo "span_lex_at_scroll=$lex_at_scroll"
  echo "span_lex_at_type=$lex_at_type"
  echo "span_lex_at_burst=$lex_at_burst"
  echo "span_rebuild_at_scroll=$rebuild_at_scroll"
  echo "span_rebuild_at_type=$rebuild_at_type"
  echo "content_frames_at_scroll=$frames_at_scroll"
  echo "content_frames_at_type=$frames_at_type"
  echo "content_frames_at_burst=$frames_at_burst"
} | tee "$REPORT_DIR/report.txt"

echo "[editor_perf_spans_visible_only] OK stall=${type_stall}ms scroll_rebuild_delta=${scroll_rebuild_delta} type_lex_avg=${type_lex_avg} (max=${TYPE_SPAN_LEX_AVG_MAX})" >&2
