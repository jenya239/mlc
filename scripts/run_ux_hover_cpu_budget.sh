#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108d — L1 counter gate + L2 /proc hover CPU%.
# L2 ceiling HOVER_CPU_BUDGET_PERCENT measured 2026-08-03 then written (honesty).
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

# Measured under MLC_EDITOR_HOVER_CPU_PROBE=1 (min of 3×2s windows): cpu_percent=0.
# Headroom → 10 (same order as idle_cpu_budget_stable default).
# Committed floor: env may raise budget, not lower (ceiling=1 would false-green at cpu=0).
COMMITTED_HOVER_CPU_BUDGET_PERCENT=10
HOVER_CPU_BUDGET_PERCENT="${HOVER_CPU_BUDGET_PERCENT:-$COMMITTED_HOVER_CPU_BUDGET_PERCENT}"
WARMUP_SEC="${HOVER_CPU_WARMUP_SEC:-4}"
SAMPLE_SEC="${HOVER_CPU_SAMPLE_SEC:-2}"
SAMPLE_ROUNDS="${HOVER_CPU_SAMPLE_ROUNDS:-3}"

fail() {
  echo "[ux hover_cpu_budget] FAIL: $1" >&2
  exit 1
}

if [ "$HOVER_CPU_BUDGET_PERCENT" -lt "$COMMITTED_HOVER_CPU_BUDGET_PERCENT" ]; then
  fail "HOVER_CPU_BUDGET_PERCENT=$HOVER_CPU_BUDGET_PERCENT below committed measured ceiling $COMMITTED_HOVER_CPU_BUDGET_PERCENT (Decision sabotage)"
fi

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF" ] || fail "missing perf.mlc"

grep -q 'content_rebuild_count' "$PERF" || fail "perf missing content_rebuild_count"
grep -q 'text_layer_rebuild_count' "$PERF" || fail "perf missing text_layer_rebuild_count"
grep -q 'chrome_rebuild_count' "$PERF" || fail "perf missing chrome_rebuild_count"
grep -q 'MLC_EDITOR_HOVER_CPU_PROBE' "$DEMO" || fail "demo_live missing hover CPU probe"
grep -q 'editor_ux_chrome_hit_pointer_plan' "$DEMO" || fail "demo_live missing hit pointer plan"

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

# --- L2: /proc CPU% under same-hit hover probe ---
if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[ux hover_cpu_budget] SKIP L2: glfw3 not found" >&2
  echo "ux_ok hover_cpu_budget"
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[ux hover_cpu_budget] SKIP L2: DejaVuSansMono.ttf not found" >&2
  echo "ux_ok hover_cpu_budget"
  exit 0
fi
if [ ! -r /proc/self/stat ]; then
  echo "[ux hover_cpu_budget] SKIP L2: /proc not available" >&2
  echo "ux_ok hover_cpu_budget"
  exit 0
fi

if [ ! -x "$DEMO_BIN" ]; then
  bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"
fi
[ -x "$DEMO_BIN" ] || fail "missing demo binary $DEMO_BIN"

read_utime_stime_jiffies() {
  local pid="$1"
  local line rest
  if [ ! -r "/proc/$pid/stat" ]; then
    echo ""
    return 1
  fi
  line="$(cat "/proc/$pid/stat")"
  rest="${line#*) }"
  set -- $rest
  echo $((${12} + ${13}))
}

HZ="$(getconf CLK_TCK 2>/dev/null || echo 100)"
if [ "$HZ" -le 0 ]; then
  HZ=100
fi

export MLC_GLFW_VISIBLE=0
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
export MLC_EDITOR_HOVER_CPU_PROBE=1

"$DEMO_BIN" >/dev/null 2>&1 &
pid=$!
cleanup() {
  kill "$pid" 2>/dev/null || true
  wait "$pid" 2>/dev/null || true
}
trap cleanup EXIT

sleep "$WARMUP_SEC"
if ! kill -0 "$pid" 2>/dev/null; then
  fail "demo exited during hover warmup"
fi

min_percent=100
round=0
while [ "$round" -lt "$SAMPLE_ROUNDS" ]; do
  start_jiffies="$(read_utime_stime_jiffies "$pid")"
  if [ -z "$start_jiffies" ]; then
    fail "cannot read /proc/$pid/stat"
  fi
  sleep "$SAMPLE_SEC"
  if ! kill -0 "$pid" 2>/dev/null; then
    fail "demo exited during hover sample"
  fi
  end_jiffies="$(read_utime_stime_jiffies "$pid")"
  if [ -z "$end_jiffies" ]; then
    fail "cannot read /proc/$pid/stat after sample"
  fi
  delta=$((end_jiffies - start_jiffies))
  if [ "$delta" -lt 0 ]; then
    delta=0
  fi
  cpu_percent=$((delta * 100 / (HZ * SAMPLE_SEC)))
  if [ "$cpu_percent" -lt "$min_percent" ]; then
    min_percent=$cpu_percent
  fi
  round=$((round + 1))
done

if [ "$min_percent" -gt "$HOVER_CPU_BUDGET_PERCENT" ]; then
  fail "hover min_cpu_percent=${min_percent} (budget=${HOVER_CPU_BUDGET_PERCENT} rounds=${SAMPLE_ROUNDS} sample=${SAMPLE_SEC}s)"
fi

echo "ux_ok hover_cpu_budget cpu_percent=${min_percent}"
echo "[ux hover_cpu_budget] ok L1+L2 cpu_percent=${min_percent} budget=${HOVER_CPU_BUDGET_PERCENT}" >&2
