#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109i — Green: sample minimap rows to strip height.
# Authority: L1 row-bound + static wire; dogfood type_stall + scroll≤60 (§109e threshold).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/minimap_sample.mlc"
OUT_DIR="${EDITOR_PERF_MINIMAP_SAMPLE_OUT:-$ROOT_DIR/.tmp/editor_perf_minimap_sample}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
DOGFOOD_REPORT_DIR="$OUT_DIR/dogfood"

TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"
SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"

fail() {
  echo "[editor_perf_minimap_sample] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing minimap_sample.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$MINIMAP" ] || fail "missing minimap.mlc"
[ -f "$GLYPH" ] || fail "missing glyph-layer budget (§109e)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline"
[ -f "$TRACK" ] || fail "missing TRACK"

grep -q 'editor_ux_minimap_sample_count' "$MINIMAP" || \
  fail "minimap.mlc missing editor_ux_minimap_sample_count"
grep -q 'editor_ux_minimap_sample_source_line' "$MINIMAP" || \
  fail "minimap.mlc missing editor_ux_minimap_sample_source_line"
grep -q 'editor_ux_minimap_sample_count' "$DEMO" || \
  fail "demo_live missing sample_count wire"

# Must not full-walk document lines for minimap_lines rebuild.
if grep -q 'while map_line < line_index_line_count(line_index)' "$DEMO"; then
  fail "demo_live still full line_count minimap rebuild loop"
fi
grep -q 'while sample_index < sample_count' "$DEMO" || \
  fail "demo_live missing sample_index loop"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
l1_output=$("$BIN_OUT" 2>&1)
l1_status=$?
set -e
printf '%s\n' "$l1_output" | tee "$OUT_DIR/l1.log"
[ "$l1_status" -eq 0 ] || fail "L1 binary exit=$l1_status"
printf '%s\n' "$l1_output" | grep -q 'ux_ok minimap_sample' || \
  fail "missing ux_ok minimap_sample"

# Dogfood side gates (same §109e scroll threshold; median-of-3 if noisy).
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
printf '%s\n' "$dog_output" | tee "$OUT_DIR/dogfood.log"
[ "$dog_status" -eq 0 ] || fail "dogfood baseline failed (exit=$dog_status)"

report="$DOGFOOD_REPORT_DIR/report.txt"
[ -f "$report" ] || fail "missing dogfood report"
scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$report" | cut -d= -f2)"
stall_ms="$(grep -E '^type_stall_ms=' "$report" | cut -d= -f2)"
[ -n "$scroll_cpu" ] || fail "missing scroll_cpu_percent"
[ -n "$stall_ms" ] || fail "missing type_stall_ms"
[ "$stall_ms" -le "$TYPE_STALL_MS_MAX" ] || fail "stall_ms=$stall_ms > $TYPE_STALL_MS_MAX"

if [ "$scroll_cpu" -gt "$SCROLL_CPU_MAX" ]; then
  scroll_samples="$scroll_cpu"
  for _retry in 1 2; do
    retry_dir="$OUT_DIR/dogfood_retry_${_retry}"
    export EDITOR_PERF_DOGFOOD_OUT="$retry_dir"
    rm -rf "$retry_dir"
    mkdir -p "$retry_dir"
    set +e
    bash "$DOGFOOD" >/dev/null 2>"$OUT_DIR/dogfood_retry_${_retry}.log"
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
  echo "[editor_perf_minimap_sample] scroll samples: $scroll_samples → median=$scroll_cpu" >&2
fi

[ "$scroll_cpu" -le "$SCROLL_CPU_MAX" ] || \
  fail "scroll_cpu=$scroll_cpu > $SCROLL_CPU_MAX"

echo "l1=minimap_sample"
echo "scroll_cpu_percent=$scroll_cpu"
echo "type_stall_ms=$stall_ms"
echo "sample_bound=strip_height"
echo "[editor_perf_minimap_sample] OK scroll=$scroll_cpu% stall=${stall_ms}ms"
