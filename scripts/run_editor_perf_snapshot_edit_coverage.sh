#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109g — Green: snapshot edit coverage.
# L2 flatten stability + demo wire for paste/cut/backspace/newline;
# dogfood stall + scroll non-regress side gates.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/snapshot_edit_coverage.mlc"
OUT_DIR="${EDITOR_PERF_SNAPSHOT_EDIT_OUT:-$ROOT_DIR/.tmp/editor_perf_snapshot_edit_coverage}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
DOGFOOD_REPORT_DIR="$OUT_DIR/dogfood"

TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"
SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"
# Allowlisted remaining force-1 ticks (open/tab/undo/multi/word-delete/…).
FORCE_ONE_MAX="${MLC_EDITOR_PERF_SNAPSHOT_FORCE_ONE_MAX:-24}"

fail() {
  echo "[editor_perf_snapshot_edit_coverage] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing snapshot_edit_coverage.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$EDIT" ] || fail "missing edit.mlc"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline"
[ -f "$TRACK" ] || fail "missing TRACK"

grep -q 'edit_apply_span_newline' "$EDIT" || fail "edit.mlc missing edit_apply_span_newline"
grep -q 'frame_layout_after_span_edit' "$DEMO" || fail "demo_live missing frame_layout_after_span_edit"

# Covered paths must call after_span_edit within the next ~12 lines.
path_uses_span_edit() {
  local needle="$1"
  local label="$2"
  if ! awk -v needle="$needle" '
    index($0, needle) {
      for (i = 1; i <= 12; i++) {
        if ((getline line) <= 0) break
        if (index(line, "frame_layout_after_span_edit(")) { found=1; exit }
        if (index(line, "frame_layout_tick_snapshot(frame_layout, document, 1)")) {
          bad=1; exit
        }
      }
    }
    END { exit((found && !bad) ? 0 : 1) }
  ' "$DEMO"; then
    fail "demo_live $label path not on span-edit"
  fi
}
path_uses_span_edit 'edit_paste(' 'paste'
path_uses_span_edit 'edit_cut(' 'cut'
if ! awk '
  /edit_apply_span_backspace\(/ { found=1 }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live missing backspace span-edit"
fi
if ! awk '
  /edit_apply_span_newline\(/ { found=1 }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live missing newline span-edit"
fi

force_one_count="$(
  grep -c 'frame_layout_tick_snapshot(frame_layout, document, 1)' "$DEMO" || true
)"
if [ "${force_one_count:-0}" -gt "$FORCE_ONE_MAX" ]; then
  fail "force-1 snapshot ticks=$force_one_count > allowlist max $FORCE_ONE_MAX"
fi

mkdir -p "$OUT_DIR"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output" | tee "$OUT_DIR/scenario.log"
[ "$status" -eq 0 ] || fail "snapshot_edit_coverage binary exit=$status"
printf '%s\n' "$output" | grep -q 'ux_ok snapshot_edit_coverage' || \
  fail "missing ux_ok snapshot_edit_coverage"

export EDITOR_PERF_DOGFOOD_OUT="$DOGFOOD_REPORT_DIR"
export MLC_EDITOR_PERF_OPEN="$DEMO"
export DOGFOOD_WARMUP_SEC="${DOGFOOD_WARMUP_SEC:-2}"
export DOGFOOD_SAMPLE_SEC_LONG="${DOGFOOD_SAMPLE_SEC_LONG:-2}"
export DOGFOOD_SAMPLE_ROUNDS_LONG="${DOGFOOD_SAMPLE_ROUNDS_LONG:-1}"
export DOGFOOD_SAMPLE_SEC_SHORT="${DOGFOOD_SAMPLE_SEC_SHORT:-2}"
export DOGFOOD_SAMPLE_ROUNDS_SHORT="${DOGFOOD_SAMPLE_ROUNDS_SHORT:-1}"

rm -rf "$DOGFOOD_REPORT_DIR"
mkdir -p "$DOGFOOD_REPORT_DIR"
set +e
dog_output="$(bash "$DOGFOOD" 2>&1)"
dog_status=$?
set -e
printf '%s\n' "$dog_output" | tee "$OUT_DIR/dogfood.log"
[ "$dog_status" -eq 0 ] || fail "dogfood failed (exit=$dog_status)"

report="$DOGFOOD_REPORT_DIR/report.txt"
[ -f "$report" ] || fail "missing dogfood report"
type_stall="$(grep -E '^type_stall_ms=' "$report" | cut -d= -f2)"
scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$report" | cut -d= -f2)"
[ -n "$type_stall" ] || fail "missing type_stall_ms"
[ -n "$scroll_cpu" ] || fail "missing scroll_cpu_percent"
if [ "$type_stall" -gt "$TYPE_STALL_MS_MAX" ]; then
  fail "type_stall_ms=$type_stall > $TYPE_STALL_MS_MAX"
fi
if [ "$scroll_cpu" -gt "$SCROLL_CPU_MAX" ]; then
  fail "scroll_cpu_percent=$scroll_cpu > $SCROLL_CPU_MAX"
fi

{
  echo "type_stall_ms=$type_stall"
  echo "scroll_cpu_percent=$scroll_cpu"
  echo "force_one_count=$force_one_count"
  echo "force_one_max=$FORCE_ONE_MAX"
  echo "l2=snapshot_edit_coverage"
} | tee "$OUT_DIR/report.txt"

echo "[editor_perf_snapshot_edit_coverage] OK stall=${type_stall}ms scroll=${scroll_cpu}% force_one=${force_one_count}" >&2
