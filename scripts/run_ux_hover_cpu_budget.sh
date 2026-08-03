#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108d L1 + TRACK_EDITOR_PERF_DOGFOOD §109c L2.
# L1: deterministic counter scenario (unchanged).
# L2: visible dogfood still-over-text (§109c honesty) — NOT sufficient alone for
# epic hover close; load-bearing proof is scripts/run_editor_perf_wake_on_hover.sh.
# Committed L2 ceiling = epic still budget 8% (env may raise, not lower).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/hover_cpu_budget.mlc"
OUT_DIR="${UX_HOVER_CPU_BUDGET_OUT:-$ROOT_DIR/tmp/ux_hover_cpu_budget}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
DEMO_OUT="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"
DEMO_BIN="$DEMO_OUT/bin"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"

# §109c: epic still-over-text budget (replaces false-green cpu=0 → ceiling 10).
COMMITTED_HOVER_CPU_BUDGET_PERCENT=8
HOVER_CPU_BUDGET_PERCENT="${HOVER_CPU_BUDGET_PERCENT:-$COMMITTED_HOVER_CPU_BUDGET_PERCENT}"

fail() {
  echo "[ux hover_cpu_budget] FAIL: $1" >&2
  exit 1
}

if [ "$HOVER_CPU_BUDGET_PERCENT" -lt "$COMMITTED_HOVER_CPU_BUDGET_PERCENT" ]; then
  fail "HOVER_CPU_BUDGET_PERCENT=$HOVER_CPU_BUDGET_PERCENT below committed ceiling $COMMITTED_HOVER_CPU_BUDGET_PERCENT"
fi

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF" ] || fail "missing perf.mlc"
[ -f "$WAKE" ] || fail "missing wake harness (hover L2 authority dependency)"

grep -q 'content_rebuild_count' "$PERF" || fail "perf missing content_rebuild_count"
grep -q 'text_layer_rebuild_count' "$PERF" || fail "perf missing text_layer_rebuild_count"
grep -q 'chrome_rebuild_count' "$PERF" || fail "perf missing chrome_rebuild_count"
grep -q 'editor_ux_chrome_hit_pointer_plan' "$DEMO" || fail "demo_live missing hit pointer plan"
grep -q 'dogfood_probe_x' "$DEMO" || fail "demo_live missing text-rect probe"

# Decision sabotage: mouse-move must not raise content_dirty.
FRAME_INPUT="$ROOT_DIR/misc/editor/app/frame_input.mlc"
mouse_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("if input.mouse_x != last_mouse_x || input.mouse_y != last_mouse_y then")
    abort "missing mouse-move dirty block" if start_at.nil?
    window_end = source.index("\n  end\n  if input.mouse_down", start_at)
    abort "missing mouse-move block end" if window_end.nil?
    print source[start_at...window_end]
  ' "$FRAME_INPUT"
)"
if printf '%s\n' "$mouse_block" | grep -q 'content_dirty = 1'; then
  fail "mouse-move raises content_dirty (Decision sabotage)"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

# --- L1: deterministic counter scenario ---
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  fail "L1 binary exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok hover_cpu_budget'; then
  fail "missing L1 ux_ok"
fi
echo "[ux hover_cpu_budget] L1 ok" >&2

# --- L2: visible still-over-text via wake harness (no VISIBLE=0 skip-green) ---
if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  fail "glfw3 not found — Decision: no L2 skip-green"
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  fail "DejaVuSansMono.ttf not found — Decision: no L2 skip-green"
fi
if [ ! -r /proc/self/stat ]; then
  fail "/proc not available — Decision: no L2 skip-green"
fi

export MLC_GLFW_VISIBLE=1
export MLC_EDITOR_PERF_OPEN="$DEMO"
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
# Short windows for hover L2; wake still asserts hit=text + ceiling ≤8.
export WAKE_SAMPLE_SEC="${HOVER_CPU_SAMPLE_SEC:-2}"
export WAKE_SAMPLE_ROUNDS="${HOVER_CPU_SAMPLE_ROUNDS:-2}"
export WAKE_SETTLE_SEC="${HOVER_CPU_SETTLE_SEC:-2}"
export WAKE_WARMUP_SEC="${HOVER_CPU_WARMUP_SEC:-3}"
export WAKE_STILL_CPU_MAX="$HOVER_CPU_BUDGET_PERCENT"
export WAKE_JITTER_CPU_MAX="${HOVER_CPU_JITTER_MAX:-15}"
export EDITOR_PERF_WAKE_OUT="${UX_HOVER_CPU_BUDGET_WAKE_OUT:-$ROOT_DIR/.tmp/ux_hover_cpu_budget_wake}"

bash "$WAKE"
report="$EDITOR_PERF_WAKE_OUT/report.txt"
[ -f "$report" ] || fail "missing wake report after L2"
still="$(grep -E '^still_over_text_cpu_percent=' "$report" | cut -d= -f2)"
[ -n "$still" ] || fail "missing still_over_text_cpu_percent in wake report"
if [ "$still" -gt "$HOVER_CPU_BUDGET_PERCENT" ]; then
  fail "hover still_over_text cpu=${still}% > budget=${HOVER_CPU_BUDGET_PERCENT}%"
fi

echo "ux_ok hover_cpu_budget cpu_percent=${still}"
echo "[ux hover_cpu_budget] ok L1+L2 still=${still}% budget=${HOVER_CPU_BUDGET_PERCENT} (wake authority; not sole epic hover green)" >&2
