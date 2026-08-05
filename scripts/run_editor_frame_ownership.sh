#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110b — Green: EditorFrame ownership + gen-stable L1.
# L1 scenario + static live wire + wake side + dogfood non-regress.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/frame_ownership_stable.mlc"
FRAME_MOD="$ROOT_DIR/misc/editor/app/editor_frame.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
RED="$ROOT_DIR/scripts/run_editor_frame_ownership_red.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
REPORT_DIR="${EDITOR_FRAME_OWNERSHIP_OUT:-$ROOT_DIR/.tmp/editor_frame_ownership}"
OUT_DIR="${UX_FRAME_OWNERSHIP_OUT:-$ROOT_DIR/tmp/ux_frame_ownership_stable}"
BIN_OUT="$OUT_DIR/bin"
REPORT_FILE="$REPORT_DIR/report.txt"
L1_LOG="$REPORT_DIR/l1.log"

fail() {
  echo "[editor_frame_ownership] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing L1 scenario $ENTRY"
[ -f "$FRAME_MOD" ] || fail "missing editor_frame.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$WAKE" ] || fail "missing wake harness"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate"
[ -f "$RED" ] || fail "missing red harness"

grep -q 'export type EditorFrame' "$FRAME_MOD" || fail "missing type EditorFrame"
grep -q 'layout_generation' "$FRAME_MOD" || fail "missing layout_generation"
grep -q 'paint_generation' "$FRAME_MOD" || fail "missing paint_generation"
grep -q "from './app/editor_frame'" "$DEMO" || fail "demo_live missing editor_frame import"
grep -q 'editor_frame_new' "$DEMO" || fail "demo_live missing editor_frame_new"
grep -q 'editor_frame_mark_paint' "$DEMO" || fail "demo_live missing mark_paint"
grep -q 'layout_generation=' "$DEMO" || fail "demo_live missing layout_generation dump"

# Load-bearing live ticks must go through EditorFrame wrappers.
if grep -n 'frame_layout_tick_' "$DEMO" | grep -v 'editor_frame_tick_' >/dev/null; then
  fail "demo_live still has bare frame_layout_tick_* (must route via editor_frame_*)"
fi
grep -q 'editor_frame_tick_snapshot(' "$DEMO" || fail "demo_live missing editor_frame_tick_snapshot"
grep -q 'editor_frame_tick_pixel(' "$DEMO" || fail "demo_live missing editor_frame_tick_pixel"

rm -rf "$REPORT_DIR" "$OUT_DIR"
mkdir -p "$REPORT_DIR" "$OUT_DIR"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

echo "[editor_frame_ownership] L1 scenario…" >&2
"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
[ -x "$BIN_OUT" ] || fail "missing L1 binary $BIN_OUT"
"$BIN_OUT" | tee "$L1_LOG"
grep -q 'ux_ok frame_ownership_stable' "$L1_LOG" || fail "L1 missing ux_ok"
grep -q 'delta_layout_generation=0' "$L1_LOG" || fail "L1 missing idle delta_layout=0"
grep -q 'delta_paint_generation=0' "$L1_LOG" || fail "L1 missing idle delta_paint=0"

# Sabotage: always-dirty in scenario already asserts non-zero; also force
# red harness to see green present.
if bash "$RED" >/tmp/editor_frame_ownership_red.out 2>&1; then
  fail "red harness unexpectedly exited 0 after Green"
fi
if ! grep -q 'already present\|already define\|already declare' /tmp/editor_frame_ownership_red.out; then
  fail "red did not report already-present (got: $(head -1 /tmp/editor_frame_ownership_red.out))"
fi

echo "[editor_frame_ownership] wake side (gen deltas + rebuild)…" >&2
WAKE_OUT="$REPORT_DIR/wake"
mkdir -p "$WAKE_OUT"
EDITOR_PERF_WAKE_OUT="$WAKE_OUT" bash "$WAKE"
WAKE_REPORT="$WAKE_OUT/report.txt"
[ -f "$WAKE_REPORT" ] || fail "missing wake report"
STILL_BEFORE="$WAKE_OUT/counters_still_before.txt"
STILL_AFTER="$WAKE_OUT/counters_still_after.txt"
[ -f "$STILL_BEFORE" ] && [ -f "$STILL_AFTER" ] || fail "missing wake still counter snapshots"

read_counter() {
  local key="$1"
  local file="$2"
  grep -E "^${key}=" "$file" | tail -1 | cut -d= -f2
}

layout_before="$(read_counter layout_generation "$STILL_BEFORE")"
layout_after="$(read_counter layout_generation "$STILL_AFTER")"
paint_before="$(read_counter paint_generation "$STILL_BEFORE")"
paint_after="$(read_counter paint_generation "$STILL_AFTER")"
[ -n "$layout_before" ] && [ -n "$layout_after" ] || fail "missing layout_generation in wake counters"
[ -n "$paint_before" ] && [ -n "$paint_after" ] || fail "missing paint_generation in wake counters"
delta_layout=$((layout_after - layout_before))
delta_paint=$((paint_after - paint_before))
if [ "$delta_layout" -ne 0 ]; then
  fail "L1 live layout_generation delta=$delta_layout (want 0)"
fi
if [ "$delta_paint" -ne 0 ]; then
  fail "L1 live paint_generation delta=$delta_paint (want 0)"
fi

echo "[editor_frame_ownership] dogfood gate (one quiet pass)…" >&2
DOGFOOD_OUT="$REPORT_DIR/dogfood"
mkdir -p "$DOGFOOD_OUT"
EDITOR_PERF_DOGFOOD_GATE_OUT="$DOGFOOD_OUT" bash "$DOGFOOD_GATE"

{
  echo "layout_generation_before=$layout_before"
  echo "layout_generation_after=$layout_after"
  echo "paint_generation_before=$paint_before"
  echo "paint_generation_after=$paint_after"
  echo "delta_layout_generation=$delta_layout"
  echo "delta_paint_generation=$delta_paint"
  echo "l1=ok"
  echo "wake=ok"
  echo "dogfood_gate=ok"
} | tee "$REPORT_FILE"

echo "[editor_frame_ownership] ok" >&2
