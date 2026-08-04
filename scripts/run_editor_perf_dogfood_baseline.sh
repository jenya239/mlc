#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109a — visible dogfood baseline (measure only).
# Shell drives phases via CMD_FILE; demo acks on PHASE_FILE + stdout.
# Fail if open path missing/README*, or glfw/font/proc absent.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
REPORT_DIR="${EDITOR_PERF_DOGFOOD_OUT:-$ROOT_DIR/.tmp/editor_perf_dogfood_baseline}"
PHASE_FILE="$REPORT_DIR/phase.txt"
CMD_FILE="$REPORT_DIR/phase_cmd.txt"
LOG_FILE="$REPORT_DIR/demo.log"
REPORT_FILE="$REPORT_DIR/report.txt"
DEMO_OUT="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"
DEMO_BIN="$DEMO_OUT/bin"
OPEN_PATH="${MLC_EDITOR_PERF_OPEN:-$DEMO}"

SAMPLE_SEC_LONG="${DOGFOOD_SAMPLE_SEC_LONG:-5}"
SAMPLE_SEC_SHORT="${DOGFOOD_SAMPLE_SEC_SHORT:-2}"
SAMPLE_ROUNDS_LONG="${DOGFOOD_SAMPLE_ROUNDS_LONG:-3}"
SAMPLE_ROUNDS_SHORT="${DOGFOOD_SAMPLE_ROUNDS_SHORT:-3}"
PHASE_WAIT_SEC="${DOGFOOD_PHASE_WAIT_SEC:-180}"
WARMUP_SEC="${DOGFOOD_WARMUP_SEC:-4}"

fail() {
  echo "[editor_perf_dogfood_baseline] FAIL: $1" >&2
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
grep -q 'MLC_EDITOR_PERF_DOGFOOD' "$DEMO" || fail "demo_live missing MLC_EDITOR_PERF_DOGFOOD probe"
grep -q 'MLC_EDITOR_PERF_DOGFOOD_CMD_FILE' "$DEMO" || fail "demo_live missing CMD_FILE drive"

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

sample_cpu() {
  local mode="$1"
  local sample_sec="$2"
  local rounds="$3"
  local best=""
  local round=0
  local start_jiffies end_jiffies delta cpu_percent
  local samples=""
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
    if [ "$mode" = "median" ]; then
      samples="$samples $cpu_percent"
    elif [ -z "$best" ]; then
      best=$cpu_percent
    elif [ "$mode" = "min" ] && [ "$cpu_percent" -lt "$best" ]; then
      best=$cpu_percent
    elif [ "$mode" = "max" ] && [ "$cpu_percent" -gt "$best" ]; then
      best=$cpu_percent
    fi
    round=$((round + 1))
  done
  if [ "$mode" = "median" ]; then
    # shellcheck disable=SC2086
    best="$(printf '%s\n' $samples | sort -n | awk -v n="$rounds" 'NR==int((n+1)/2) { print; exit }')"
  fi
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

export MLC_GLFW_VISIBLE=1
export MLC_EDITOR_PERF_DOGFOOD=1
export MLC_EDITOR_PERF_OPEN="$OPEN_PATH"
export MLC_EDITOR_PERF_DOGFOOD_PHASE_FILE="$PHASE_FILE"
export MLC_EDITOR_PERF_DOGFOOD_CMD_FILE="$CMD_FILE"

: >"$PHASE_FILE"
: >"$CMD_FILE"
: >"$LOG_FILE"
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
idle_away_cpu_percent="$(sample_cpu min "$SAMPLE_SEC_LONG" "$SAMPLE_ROUNDS_LONG")"

set_phase_cmd "still_over_text"
wait_phase_marker "phase=still_over_text"
still_over_text_cpu_percent="$(sample_cpu min "$SAMPLE_SEC_LONG" "$SAMPLE_ROUNDS_LONG")"

set_phase_cmd "text_jitter"
wait_phase_marker "phase=text_jitter"
text_jitter_cpu_percent="$(sample_cpu min "$SAMPLE_SEC_LONG" "$SAMPLE_ROUNDS_LONG")"

set_phase_cmd "scroll"
wait_phase_marker "phase=scroll"
# Median of short rounds (not max): max-of-2s spikes noise past ≤60 under shared load.
scroll_cpu_percent="$(sample_cpu median "$SAMPLE_SEC_SHORT" "$SAMPLE_ROUNDS_SHORT")"

set_phase_cmd "type"
wait_phase_marker "phase=type"
wait_phase_marker "type_burst_done"
type_cpu_percent="$(sample_cpu median "$SAMPLE_SEC_SHORT" "$SAMPLE_ROUNDS_SHORT")"

wait_phase_marker "type_stall_ms="
type_stall_ms="$(grep -o 'type_stall_ms=[0-9]*' "$LOG_FILE" "$PHASE_FILE" 2>/dev/null | tail -1 | cut -d= -f2)"
[ -n "$type_stall_ms" ] || fail "missing type_stall_ms"

set_phase_cmd "quit"
wait_phase_marker "done" || true

{
  echo "idle_away_cpu_percent=$idle_away_cpu_percent"
  echo "still_over_text_cpu_percent=$still_over_text_cpu_percent"
  echo "text_jitter_cpu_percent=$text_jitter_cpu_percent"
  echo "scroll_cpu_percent=$scroll_cpu_percent"
  echo "type_cpu_percent=$type_cpu_percent"
  echo "type_stall_ms=$type_stall_ms"
  echo "open_path=$OPEN_PATH"
} >"$REPORT_FILE"

echo "editor_perf_dogfood_baseline_ok"
echo "[editor_perf_dogfood_baseline] idle_away=${idle_away_cpu_percent}% still=${still_over_text_cpu_percent}% jitter=${text_jitter_cpu_percent}% scroll=${scroll_cpu_percent}% type=${type_cpu_percent}% stall_ms=${type_stall_ms}" >&2
cat "$REPORT_FILE"
