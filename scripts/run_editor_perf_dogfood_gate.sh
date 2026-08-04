#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109k — Green: composite dogfood regression suite.
# Compose (do not rewrite) honesty → wake → dogfood+ceilings → glyph → tree →
# minimap → startup. Fail-fast. ×2 before Critic close.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
REPORT_DIR="${EDITOR_PERF_DOGFOOD_GATE_OUT:-$ROOT_DIR/.tmp/editor_perf_dogfood_gate}"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
OPEN_PATH="${MLC_EDITOR_PERF_OPEN:-$DEMO}"
DEMO_OUT="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"

HONESTY="$ROOT_DIR/scripts/run_editor_perf_gate_honesty.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
TREE="$ROOT_DIR/scripts/run_editor_perf_chrome_tree_visible.sh"
MINIMAP="$ROOT_DIR/scripts/run_editor_perf_minimap_sample.sh"
STARTUP="$ROOT_DIR/scripts/run_editor_perf_startup_open.sh"

IDLE_AWAY_CPU_MAX="${MLC_EDITOR_PERF_IDLE_AWAY_CPU_MAX:-5}"
STILL_CPU_MAX="${MLC_EDITOR_PERF_STILL_OVER_CPU_MAX:-8}"
JITTER_CPU_MAX="${MLC_EDITOR_PERF_JITTER_CPU_MAX:-15}"
SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"
TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"

fail() {
  echo "[editor_perf_dogfood_gate] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$HONESTY" ] || fail "missing gate honesty (§109c)"
[ -f "$WAKE" ] || fail "missing wake (§109b)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (§109a)"
[ -f "$GLYPH" ] || fail "missing glyph-layer (§109e)"
[ -f "$TREE" ] || fail "missing chrome-tree (§109h)"
[ -f "$MINIMAP" ] || fail "missing minimap-sample (§109i)"
[ -f "$STARTUP" ] || fail "missing startup-open (§109j)"

open_basename="$(basename "$OPEN_PATH")"
case "$open_basename" in
  README*) fail "open path basename is README* ($open_basename)" ;;
esac
[ -f "$OPEN_PATH" ] || fail "open path missing: $OPEN_PATH"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  fail "glfw3 not found — Decision: no skip-green"
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  fail "DejaVuSansMono.ttf not found — Decision: no skip-green"
fi
if [ ! -r /proc/self/stat ]; then
  fail "/proc not available — Decision: no skip-green"
fi

SKIP_RAW="${EDITOR_PERF_DOGFOOD_GATE_SKIP:-}"
if [ -n "$SKIP_RAW" ] && [ "${EDITOR_PERF_DOGFOOD_GATE_ALLOW_SKIP:-0}" != "1" ]; then
  fail "EDITOR_PERF_DOGFOOD_GATE_SKIP set without EDITOR_PERF_DOGFOOD_GATE_ALLOW_SKIP=1"
fi

should_skip() {
  local name="$1"
  [ -z "$SKIP_RAW" ] && return 1
  case ",$SKIP_RAW," in
    *",$name,"*) return 0 ;;
    *) return 1 ;;
  esac
}

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
export MLC_GLFW_VISIBLE=1
export MLC_EDITOR_PERF_OPEN="$OPEN_PATH"
export EDITOR_DEMO_LIVE_FS_OUT="$DEMO_OUT"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
unset MLC_EDITOR_PERF || true

rm -rf "$REPORT_DIR"
mkdir -p "$REPORT_DIR"
REPORT_FILE="$REPORT_DIR/report.txt"
: >"$REPORT_FILE"

member_ok() {
  echo "member=$1 status=ok" | tee -a "$REPORT_FILE"
}

member_skip() {
  echo "member=$1 status=skip" | tee -a "$REPORT_FILE"
}

run_member() {
  local name="$1"
  shift
  if should_skip "$name"; then
    member_skip "$name"
    return 0
  fi
  local member_dir="$REPORT_DIR/$name"
  mkdir -p "$member_dir"
  echo "[editor_perf_dogfood_gate] RUN $name" >&2
  set +e
  "$@" >"$member_dir/log.txt" 2>&1
  local status=$?
  set -e
  if [ "$status" -ne 0 ]; then
    echo "member=$name status=fail exit=$status" | tee -a "$REPORT_FILE"
    tail -n 40 "$member_dir/log.txt" >&2 || true
    fail "member $name failed (exit=$status) — see $member_dir/log.txt"
  fi
  member_ok "$name"
  export MLCC_OBJ_CLEAN=0
}

assert_dogfood_ceilings() {
  local report="$1"
  [ -f "$report" ] || fail "dogfood missing report.txt"
  local idle still jitter scroll stall
  idle="$(grep -E '^idle_away_cpu_percent=' "$report" | cut -d= -f2)"
  still="$(grep -E '^still_over_text_cpu_percent=' "$report" | cut -d= -f2)"
  jitter="$(grep -E '^text_jitter_cpu_percent=' "$report" | cut -d= -f2)"
  scroll="$(grep -E '^scroll_cpu_percent=' "$report" | cut -d= -f2)"
  stall="$(grep -E '^type_stall_ms=' "$report" | cut -d= -f2)"
  [ -n "$idle" ] && [ -n "$still" ] && [ -n "$jitter" ] || fail "dogfood report missing idle/still/jitter"
  [ -n "$scroll" ] && [ -n "$stall" ] || fail "dogfood report missing scroll/stall"
  [ "$idle" -le "$IDLE_AWAY_CPU_MAX" ] || fail "idle_away_cpu=$idle > $IDLE_AWAY_CPU_MAX"
  [ "$still" -le "$STILL_CPU_MAX" ] || fail "still_over_text_cpu=$still > $STILL_CPU_MAX"
  [ "$jitter" -le "$JITTER_CPU_MAX" ] || fail "text_jitter_cpu=$jitter > $JITTER_CPU_MAX"
  [ "$stall" -le "$TYPE_STALL_MS_MAX" ] || fail "type_stall_ms=$stall > $TYPE_STALL_MS_MAX"
  if [ "$scroll" -gt "$SCROLL_CPU_MAX" ]; then
    local samples="$scroll" retry retry_dir retry_scroll retry_status
    for retry in 1 2; do
      retry_dir="$REPORT_DIR/dogfood_scroll_retry_$retry"
      export EDITOR_PERF_DOGFOOD_OUT="$retry_dir"
      rm -rf "$retry_dir"
      mkdir -p "$retry_dir"
      set +e
      bash "$DOGFOOD" >"$REPORT_DIR/dogfood_scroll_retry_$retry.log" 2>&1
      retry_status=$?
      set -e
      [ "$retry_status" -eq 0 ] || fail "dogfood scroll retry $retry failed"
      retry_scroll="$(grep -E '^scroll_cpu_percent=' "$retry_dir/report.txt" | cut -d= -f2)"
      samples="$samples $retry_scroll"
    done
    scroll="$(printf '%s\n' $samples | sort -n | awk 'NR==2 { print; exit }')"
    echo "[editor_perf_dogfood_gate] scroll samples: $samples → median=$scroll" >&2
  fi
  [ "$scroll" -le "$SCROLL_CPU_MAX" ] || fail "scroll_cpu=$scroll > $SCROLL_CPU_MAX"
  {
    echo "idle_away_cpu_percent=$idle"
    echo "still_over_text_cpu_percent=$still"
    echo "text_jitter_cpu_percent=$jitter"
    echo "scroll_cpu_percent=$scroll"
    echo "type_stall_ms=$stall"
  } >>"$REPORT_FILE"
}

run_member honesty \
  env EDITOR_PERF_HONESTY_OUT="$REPORT_DIR/honesty" \
      EDITOR_PERF_WAKE_OUT="$REPORT_DIR/honesty/wake" \
      EDITOR_DEMO_LIVE_PERF_FULL_OUT="$REPORT_DIR/honesty/perf_full" \
      bash "$HONESTY"
if [ -f "$REPORT_DIR/honesty/report.txt" ]; then
  grep -E '^(still_over_text_cpu_percent|text_jitter_cpu_percent|TOTAL_US_MAX|remeasure_total_us)=' \
    "$REPORT_DIR/honesty/report.txt" >>"$REPORT_FILE" || true
fi

run_member wake \
  env EDITOR_PERF_WAKE_OUT="$REPORT_DIR/wake" \
      bash "$WAKE"
if [ -f "$REPORT_DIR/wake/report.txt" ]; then
  grep -E '^(still_over_text_cpu_percent|text_jitter_cpu_percent)=' \
    "$REPORT_DIR/wake/report.txt" | sed 's/^/wake_/' >>"$REPORT_FILE" || true
fi

if should_skip dogfood; then
  member_skip dogfood
else
  export EDITOR_PERF_DOGFOOD_OUT="$REPORT_DIR/dogfood"
  rm -rf "$EDITOR_PERF_DOGFOOD_OUT"
  mkdir -p "$EDITOR_PERF_DOGFOOD_OUT"
  echo "[editor_perf_dogfood_gate] RUN dogfood" >&2
  set +e
  bash "$DOGFOOD" >"$REPORT_DIR/dogfood/log.txt" 2>&1
  dog_status=$?
  set -e
  if [ "$dog_status" -ne 0 ]; then
    echo "member=dogfood status=fail exit=$dog_status" | tee -a "$REPORT_FILE"
    tail -n 40 "$REPORT_DIR/dogfood/log.txt" >&2 || true
    fail "member dogfood failed (exit=$dog_status)"
  fi
  assert_dogfood_ceilings "$REPORT_DIR/dogfood/report.txt"
  member_ok dogfood
  export MLCC_OBJ_CLEAN=0
fi

run_member glyph \
  env EDITOR_PERF_GLYPH_LAYER_OUT="$REPORT_DIR/glyph" \
      bash "$GLYPH"

run_member tree \
  env EDITOR_PERF_CHROME_TREE_OUT="$REPORT_DIR/tree" \
      bash "$TREE"

run_member minimap \
  env EDITOR_PERF_MINIMAP_SAMPLE_OUT="$REPORT_DIR/minimap" \
      bash "$MINIMAP"

run_member startup \
  env EDITOR_PERF_STARTUP_OPEN_OUT="$REPORT_DIR/startup" \
      bash "$STARTUP"
if [ -f "$REPORT_DIR/startup/report.txt" ]; then
  grep -E '^(open_path|time_to_first_present_ms|scroll_cpu_percent|type_stall_ms)=' \
    "$REPORT_DIR/startup/report.txt" | sed 's/^/startup_/' >>"$REPORT_FILE" || true
fi

for required in honesty wake dogfood glyph tree minimap startup; do
  grep -q "member=$required status=" "$REPORT_FILE" || \
    fail "composite report missing member=$required"
done

echo "open_path=$OPEN_PATH" >>"$REPORT_FILE"
echo "[editor_perf_dogfood_gate] OK all members" >&2
cat "$REPORT_FILE"
