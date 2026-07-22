#!/usr/bin/env bash
# TRACK_EDITOR_IDLE_CPU_BUDGET — live /proc idle CPU% under budget.
# STEP=2: paint-only caret overlay (no full-frame clear) + live /proc sample.
# CPU% = 100 * delta(utime+stime) / (HZ * SAMPLE_SEC); budget default 10%.
# Uses min of SAMPLE_ROUNDS windows after warmup (host noise / startup spikes).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
OUT_DIR="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"
BIN_OUT="$OUT_DIR/bin"
WARMUP_SEC="${IDLE_CPU_WARMUP_SEC:-4}"
SAMPLE_SEC="${IDLE_CPU_SAMPLE_SEC:-2}"
SAMPLE_ROUNDS="${IDLE_CPU_SAMPLE_ROUNDS:-3}"
BUDGET_PERCENT="${IDLE_CPU_BUDGET_PERCENT:-10}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[ux idle_cpu_budget_stable] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[ux idle_cpu_budget_stable] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -r /proc/self/stat ]; then
  echo "[ux idle_cpu_budget_stable] SKIP: /proc not available" >&2
  exit 0
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE=0
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"

if [ ! -x "$BIN_OUT" ]; then
  if [ ! -x "$MLCC" ]; then
    echo "[ux idle_cpu_budget_stable] FAIL: mlcc not found at $MLCC" >&2
    exit 1
  fi
  bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"
fi
if [ ! -x "$BIN_OUT" ]; then
  echo "[ux idle_cpu_budget_stable] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi

read_utime_stime_jiffies() {
  local pid="$1"
  local line rest
  if [ ! -r "/proc/$pid/stat" ]; then
    echo ""
    return 1
  fi
  line="$(cat "/proc/$pid/stat")"
  rest="${line#*) }"
  # After "(comm)": utime/stime are fields 12/13 of the remainder (stat fields 14/15).
  set -- $rest
  echo $((${12} + ${13}))
}

HZ="$(getconf CLK_TCK 2>/dev/null || echo 100)"
if [ "$HZ" -le 0 ]; then
  HZ=100
fi

"$BIN_OUT" >/dev/null 2>&1 &
pid=$!
cleanup() {
  kill "$pid" 2>/dev/null || true
  wait "$pid" 2>/dev/null || true
}
trap cleanup EXIT

sleep "$WARMUP_SEC"
if ! kill -0 "$pid" 2>/dev/null; then
  echo "[ux idle_cpu_budget_stable] FAIL: demo exited during warmup" >&2
  exit 1
fi

min_percent=100
round=0
while [ "$round" -lt "$SAMPLE_ROUNDS" ]; do
  start_jiffies="$(read_utime_stime_jiffies "$pid")"
  if [ -z "$start_jiffies" ]; then
    echo "[ux idle_cpu_budget_stable] FAIL: cannot read /proc/$pid/stat" >&2
    exit 1
  fi
  sleep "$SAMPLE_SEC"
  if ! kill -0 "$pid" 2>/dev/null; then
    echo "[ux idle_cpu_budget_stable] FAIL: demo exited during sample" >&2
    exit 1
  fi
  end_jiffies="$(read_utime_stime_jiffies "$pid")"
  if [ -z "$end_jiffies" ]; then
    echo "[ux idle_cpu_budget_stable] FAIL: cannot read /proc/$pid/stat after sample" >&2
    exit 1
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

# min_percent is the lowest window; must stay under budget.
if [ "$min_percent" -ge "$BUDGET_PERCENT" ]; then
  echo "[ux idle_cpu_budget_stable] FAIL: idle min_cpu_percent=${min_percent} (budget=${BUDGET_PERCENT} rounds=${SAMPLE_ROUNDS} sample=${SAMPLE_SEC}s)" >&2
  exit 1
fi

echo "ux_ok idle_cpu_budget_stable cpu_percent=${min_percent}"
echo "[ux idle_cpu_budget_stable] ok cpu_percent=${min_percent} budget=${BUDGET_PERCENT}" >&2
