#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109b — wake-on-hover L1/L2 (text-rect probe).
# VISIBLE=1 + DOGFOOD + WAKE_PROBE; no PERF skip-heavy.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
REPORT_DIR="${EDITOR_PERF_WAKE_OUT:-$ROOT_DIR/.tmp/editor_perf_wake_on_hover}"
PHASE_FILE="$REPORT_DIR/phase.txt"
CMD_FILE="$REPORT_DIR/phase_cmd.txt"
COUNTERS_FILE="$REPORT_DIR/counters.txt"
LOG_FILE="$REPORT_DIR/demo.log"
REPORT_FILE="$REPORT_DIR/report.txt"
DEMO_OUT="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"
DEMO_BIN="$DEMO_OUT/bin"
OPEN_PATH="${MLC_EDITOR_PERF_OPEN:-$DEMO}"

SAMPLE_SEC="${WAKE_SAMPLE_SEC:-5}"
SAMPLE_ROUNDS="${WAKE_SAMPLE_ROUNDS:-3}"
SETTLE_SEC="${WAKE_SETTLE_SEC:-2}"
PHASE_WAIT_SEC="${WAKE_PHASE_WAIT_SEC:-180}"
WARMUP_SEC="${WAKE_WARMUP_SEC:-4}"
STILL_CPU_MAX="${WAKE_STILL_CPU_MAX:-8}"
JITTER_CPU_MAX="${WAKE_JITTER_CPU_MAX:-15}"

fail() {
  echo "[editor_perf_wake_on_hover] FAIL: $1" >&2
  exit 1
}

open_basename="$(basename "$OPEN_PATH")"
case "$open_basename" in
  README*) fail "open path basename is README* ($open_basename)" ;;
esac
[ -f "$OPEN_PATH" ] || fail "open path missing: $OPEN_PATH"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  fail "glfw3 not found (pkg-config) — Decision: no skip-green"
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  fail "DejaVuSansMono.ttf not found — Decision: no skip-green"
fi
if [ ! -r /proc/self/stat ]; then
  fail "/proc not available — Decision: no skip-green"
fi

grep -q 'MLC_EDITOR_PERF_DOGFOOD' "$DEMO" || fail "demo_live missing MLC_EDITOR_PERF_DOGFOOD"
grep -q 'MLC_EDITOR_PERF_WAKE_PROBE' "$DEMO" || fail "demo_live missing MLC_EDITOR_PERF_WAKE_PROBE"
grep -q 'content_frame_count' "$ROOT_DIR/misc/editor/ui/perf.mlc" || fail "perf missing content_frame_count"
grep -q 'idle_wait_count' "$ROOT_DIR/misc/editor/ui/perf.mlc" || fail "perf missing idle_wait_count"
grep -q 'dogfood_probe_x' "$DEMO" || fail "demo_live missing dogfood_probe_x (text-rect probe)"
if grep -q 'dogfood_text_x = 420' "$DEMO"; then
  fail "demo_live still has hardcoded dogfood_text_x = 420"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"

rm -rf "$REPORT_DIR"
mkdir -p "$REPORT_DIR"

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"
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

sample_cpu_min() {
  local sample_sec="$1"
  local rounds="$2"
  local best=""
  local round=0
  local start_jiffies end_jiffies delta cpu_percent
  while [ "$round" -lt "$rounds" ]; do
    start_jiffies="$(read_utime_stime_jiffies "$pid")"
    if [ -z "$start_jiffies" ]; then
      fail "cannot read /proc/$pid/stat"
    fi
    sleep "$sample_sec"
    if ! kill -0 "$pid" 2>/dev/null; then
      fail "demo exited during CPU sample"
    fi
    end_jiffies="$(read_utime_stime_jiffies "$pid")"
    if [ -z "$end_jiffies" ]; then
      fail "cannot read /proc/$pid/stat after sample"
    fi
    delta=$((end_jiffies - start_jiffies))
    if [ "$delta" -lt 0 ]; then
      delta=0
    fi
    cpu_percent=$((delta * 100 / (HZ * sample_sec)))
    if [ -z "$best" ] || [ "$cpu_percent" -lt "$best" ]; then
      best=$cpu_percent
    fi
    round=$((round + 1))
  done
  echo "$best"
}

wait_phase_marker() {
  local needle="$1"
  local deadline=$(( $(date +%s) + PHASE_WAIT_SEC ))
  while [ "$(date +%s)" -lt "$deadline" ]; do
    if [ -f "$PHASE_FILE" ] && grep -q "$needle" "$PHASE_FILE"; then
      return 0
    fi
    if [ -f "$LOG_FILE" ] && grep -q "\[dogfood\] ${needle}" "$LOG_FILE"; then
      return 0
    fi
    if ! kill -0 "$pid" 2>/dev/null; then
      fail "demo exited before marker $needle (see $LOG_FILE)"
    fi
    sleep 0.2
  done
  fail "timeout waiting for $needle (see $LOG_FILE)"
}

set_phase_cmd() {
  printf '%s\n' "$1" >"$CMD_FILE"
}

read_counter() {
  local key="$1"
  local file="$2"
  if [ ! -f "$file" ]; then
    echo ""
    return 1
  fi
  grep -E "^${key}=" "$file" | tail -1 | cut -d= -f2
}

snapshot_counters() {
  local label="$1"
  local out="$REPORT_DIR/counters_${label}.txt"
  local deadline=$(( $(date +%s) + 30 ))
  while [ "$(date +%s)" -lt "$deadline" ]; do
    if [ -f "$COUNTERS_FILE" ] && grep -q 'content_rebuild_count=' "$COUNTERS_FILE"; then
      cp "$COUNTERS_FILE" "$out"
      echo "$out"
      return 0
    fi
    sleep 0.1
  done
  fail "timeout waiting for counters file ($label)"
}

counter_delta() {
  local key="$1"
  local before="$2"
  local after="$3"
  local a b
  a="$(read_counter "$key" "$before")"
  b="$(read_counter "$key" "$after")"
  [ -n "$a" ] && [ -n "$b" ] || fail "missing $key in counter snapshots"
  echo $((b - a))
}

export MLC_GLFW_VISIBLE=1
export MLC_EDITOR_PERF_DOGFOOD=1
export MLC_EDITOR_PERF_WAKE_PROBE=1
export MLC_EDITOR_PERF_OPEN="$OPEN_PATH"
export MLC_EDITOR_PERF_DOGFOOD_PHASE_FILE="$PHASE_FILE"
export MLC_EDITOR_PERF_DOGFOOD_CMD_FILE="$CMD_FILE"
export MLC_EDITOR_PERF_WAKE_COUNTERS_FILE="$COUNTERS_FILE"
# Explicitly unset skip-heavy probes.
unset MLC_EDITOR_PERF || true
unset MLC_EDITOR_PERF_FULL || true
unset MLC_EDITOR_TERMINAL_PERF || true

: >"$PHASE_FILE"
: >"$CMD_FILE"
: >"$LOG_FILE"
: >"$COUNTERS_FILE"
"$DEMO_BIN" >>"$LOG_FILE" 2>&1 &
pid=$!
cleanup() {
  set_phase_cmd quit 2>/dev/null || true
  kill "$pid" 2>/dev/null || true
  wait "$pid" 2>/dev/null || true
}
trap cleanup EXIT

wait_phase_marker "phase=warmup"
sleep "$WARMUP_SEC"

set_phase_cmd "idle_away"
wait_phase_marker "phase=idle_away"
sleep 1

set_phase_cmd "still_over_text"
wait_phase_marker "phase=still_over_text"
sleep "$SETTLE_SEC"

# Prove probe hits text chrome-hit id.
if ! grep -q '\[wake\] hit=text' "$LOG_FILE"; then
  fail "still_over_text never reported [wake] hit=text (see $LOG_FILE)"
fi

still_before="$(snapshot_counters still_before)"
sleep "$SAMPLE_SEC"
still_after="$(snapshot_counters still_after)"

delta_content_rebuild="$(counter_delta content_rebuild_count "$still_before" "$still_after")"
delta_chrome_rebuild="$(counter_delta chrome_rebuild_count "$still_before" "$still_after")"
delta_text_layer="$(counter_delta text_layer_rebuild_count "$still_before" "$still_after")"
delta_content_frame="$(counter_delta content_frame_count "$still_before" "$still_after")"
delta_chrome_frame="$(counter_delta chrome_frame_count "$still_before" "$still_after")"

if [ "$delta_content_rebuild" -ne 0 ]; then
  fail "L1 content_rebuild_count delta=$delta_content_rebuild (want 0)"
fi
if [ "$delta_chrome_rebuild" -ne 0 ]; then
  fail "L1 chrome_rebuild_count delta=$delta_chrome_rebuild (want 0)"
fi
if [ "$delta_text_layer" -ne 0 ]; then
  fail "L1 text_layer_rebuild_count delta=$delta_text_layer (want 0)"
fi
if [ "$delta_content_frame" -ne 0 ]; then
  fail "L1 content_frame_count delta=$delta_content_frame (want 0)"
fi
if [ "$delta_chrome_frame" -ne 0 ]; then
  fail "L1 chrome_frame_count delta=$delta_chrome_frame (want 0)"
fi

still_over_text_cpu_percent="$(sample_cpu_min "$SAMPLE_SEC" "$SAMPLE_ROUNDS")"
if [ "$still_over_text_cpu_percent" -gt "$STILL_CPU_MAX" ]; then
  fail "L2 still_over_text cpu=${still_over_text_cpu_percent}% > ${STILL_CPU_MAX}%"
fi

set_phase_cmd "text_jitter"
wait_phase_marker "phase=text_jitter"
sleep "$SETTLE_SEC"
text_jitter_cpu_percent="$(sample_cpu_min "$SAMPLE_SEC" "$SAMPLE_ROUNDS")"
if [ "$text_jitter_cpu_percent" -gt "$JITTER_CPU_MAX" ]; then
  fail "L2 text_jitter cpu=${text_jitter_cpu_percent}% > ${JITTER_CPU_MAX}%"
fi

set_phase_cmd "quit"
wait_phase_marker "done" || true

hit_line="$(grep '\[wake\] hit=' "$LOG_FILE" | tail -1 || true)"

{
  echo "still_over_text_cpu_percent=$still_over_text_cpu_percent"
  echo "text_jitter_cpu_percent=$text_jitter_cpu_percent"
  echo "delta_content_rebuild_count=$delta_content_rebuild"
  echo "delta_chrome_rebuild_count=$delta_chrome_rebuild"
  echo "delta_text_layer_rebuild_count=$delta_text_layer"
  echo "delta_content_frame_count=$delta_content_frame"
  echo "delta_chrome_frame_count=$delta_chrome_frame"
  echo "hit_proof=$hit_line"
  echo "open_path=$OPEN_PATH"
} >"$REPORT_FILE"

echo "editor_perf_wake_on_hover_ok"
echo "[editor_perf_wake_on_hover] still=${still_over_text_cpu_percent}% jitter=${text_jitter_cpu_percent}% L1 deltas content/chrome/text/content_frame/chrome_frame=0" >&2
cat "$REPORT_FILE"
