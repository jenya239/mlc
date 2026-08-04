#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109j — Green: OPEN resolve + first-present before full wrap.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
REPORT_DIR="${EDITOR_PERF_STARTUP_OPEN_OUT:-$ROOT_DIR/.tmp/editor_perf_startup_open}"
DEMO_OUT="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"
DEMO_BIN="$DEMO_OUT/bin"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
SESSION_FILE="$ROOT_DIR/.tmp/editor_live_session.txt"
OPEN_TARGET="$ROOT_DIR/misc/editor/demo_live.mlc"

# Aspirational interactive ceiling (Decision); Green may tighten via measured×1.25.
TIME_MAX_MS="${MLC_EDITOR_PERF_STARTUP_PRESENT_MS_MAX:-3000}"
TYPE_STALL_MS_MAX="${MLC_EDITOR_PERF_TYPE_STALL_MS_MAX:-500}"
SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"

fail() {
  echo "[editor_perf_startup_open] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline"
[ -f "$TRACK" ] || fail "missing TRACK"
[ -f "$OPEN_TARGET" ] || fail "missing open target"

grep -q 'MLC_EDITOR_OPEN' "$DEMO" || fail "demo_live missing MLC_EDITOR_OPEN"
grep -q 'resolve_startup_tabs' "$DEMO" || fail "demo_live missing resolve_startup_tabs"
grep -q 'time_to_first_present_ms' "$DEMO" || fail "demo_live missing time_to_first_present_ms"
grep -q 'startup_full_wrap_deferred' "$DEMO" || fail "demo_live missing startup_full_wrap_deferred"
grep -q 'startup_defer_full_wrap' "$DEMO" || fail "demo_live missing startup_defer_full_wrap"

# Starter must try demo_live before README.
if ! awk '
  /fn open_disk_starter\(/ { in_fn=1 }
  in_fn && /try_open_path\(tabs, root \+ "\/misc\/editor\/demo_live\.mlc"\)/ { demo=1 }
  in_fn && /try_open_path\(tabs, root \+ "\/README\.md"\)/ {
    if (!demo) { early_readme=1 }
  }
  in_fn && /^fn / && !/fn open_disk_starter\(/ { in_fn=0 }
  END { exit((demo && !early_readme) ? 0 : 1) }
' "$DEMO"; then
  fail "open_disk_starter still prefers README before demo_live"
fi

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  fail "glfw3 not found — Decision: no skip-green"
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  fail "DejaVuSansMono.ttf not found — Decision: no skip-green"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_EDITOR_ROOT="$ROOT_DIR"
export MLC_GLFW_VISIBLE=1

rm -rf "$REPORT_DIR"
mkdir -p "$REPORT_DIR"
# Empty session so starter / OPEN are load-bearing.
rm -f "$SESSION_FILE"
: > "$SESSION_FILE"

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"
[ -x "$DEMO_BIN" ] || fail "missing demo binary $DEMO_BIN"

run_startup() {
  local label="$1"
  shift
  local log="$REPORT_DIR/${label}.log"
  set +e
  "$@" >"$log" 2>&1
  local status=$?
  set -e
  [ "$status" -eq 0 ] || fail "$label binary exit=$status (see $log)"
  grep -q 'time_to_first_present_ms=' "$log" || fail "$label missing time_to_first_present_ms"
  grep -q 'startup_full_wrap_deferred=1' "$log" || fail "$label missing startup_full_wrap_deferred=1"
  grep -q '^open_path=' "$log" || fail "$label missing open_path"
  local open_path present_ms
  open_path="$(sed -n 's/^open_path=//p' "$log" | tail -n1)"
  present_ms="$(sed -n 's/^time_to_first_present_ms=//p' "$log" | tail -n1)"
  [ -n "$open_path" ] || fail "$label empty open_path"
  [ -n "$present_ms" ] || fail "$label empty present_ms"
  case "$(basename "$open_path")" in
    README*) fail "$label open_path is README*: $open_path" ;;
  esac
  [ "$present_ms" -le "$TIME_MAX_MS" ] || \
    fail "$label time_to_first_present_ms=$present_ms > $TIME_MAX_MS"
  echo "$open_path|$present_ms"
}

# (1) MLC_EDITOR_OPEN → demo_live.mlc
rm -f "$SESSION_FILE"
: > "$SESSION_FILE"
result_open="$(
  run_startup open \
    env MLC_EDITOR_PERF_STARTUP=1 MLC_EDITOR_OPEN="$OPEN_TARGET" \
      MLC_EDITOR_ROOT="$ROOT_DIR" MLC_GLFW_VISIBLE=1 "$DEMO_BIN"
)"
open_path_open="${result_open%%|*}"
present_open="${result_open##*|}"
case "$open_path_open" in
  *demo_live.mlc) ;;
  *) fail "OPEN run open_path not demo_live.mlc: $open_path_open" ;;
esac

# (2) OPEN unset + empty session → starter non-README (demo_live preferred)
rm -f "$SESSION_FILE"
: > "$SESSION_FILE"
result_starter="$(
  run_startup starter \
    env MLC_EDITOR_PERF_STARTUP=1 \
      MLC_EDITOR_ROOT="$ROOT_DIR" MLC_GLFW_VISIBLE=1 "$DEMO_BIN"
)"
open_path_starter="${result_starter%%|*}"
present_starter="${result_starter##*|}"
case "$(basename "$open_path_starter")" in
  README*) fail "starter open_path is README*: $open_path_starter" ;;
esac
case "$open_path_starter" in
  *demo_live.mlc) ;;
  *) fail "starter open_path not demo_live.mlc: $open_path_starter" ;;
esac

# Side gates: dogfood stall + scroll (short samples).
export EDITOR_PERF_DOGFOOD_OUT="$REPORT_DIR/dogfood"
export MLC_EDITOR_PERF_OPEN="$OPEN_TARGET"
export DOGFOOD_WARMUP_SEC="${DOGFOOD_WARMUP_SEC:-2}"
export DOGFOOD_SAMPLE_SEC_LONG="${DOGFOOD_SAMPLE_SEC_LONG:-2}"
export DOGFOOD_SAMPLE_ROUNDS_LONG="${DOGFOOD_SAMPLE_ROUNDS_LONG:-1}"
export DOGFOOD_SAMPLE_SEC_SHORT="${DOGFOOD_SAMPLE_SEC_SHORT:-2}"
export DOGFOOD_SAMPLE_ROUNDS_SHORT="${DOGFOOD_SAMPLE_ROUNDS_SHORT:-2}"
rm -rf "$EDITOR_PERF_DOGFOOD_OUT"
mkdir -p "$EDITOR_PERF_DOGFOOD_OUT"
set +e
dog_output="$(bash "$DOGFOOD" 2>&1)"
dog_status=$?
set -e
printf '%s\n' "$dog_output" | tee "$REPORT_DIR/dogfood.log"
[ "$dog_status" -eq 0 ] || fail "dogfood failed (exit=$dog_status)"
scroll_cpu="$(grep -E '^scroll_cpu_percent=' "$EDITOR_PERF_DOGFOOD_OUT/report.txt" | cut -d= -f2)"
stall_ms="$(grep -E '^type_stall_ms=' "$EDITOR_PERF_DOGFOOD_OUT/report.txt" | cut -d= -f2)"
[ -n "$scroll_cpu" ] || fail "missing scroll_cpu_percent"
[ -n "$stall_ms" ] || fail "missing type_stall_ms"
[ "$stall_ms" -le "$TYPE_STALL_MS_MAX" ] || fail "stall_ms=$stall_ms > $TYPE_STALL_MS_MAX"
if [ "$scroll_cpu" -gt "$SCROLL_CPU_MAX" ]; then
  scroll_samples="$scroll_cpu"
  for _retry in 1 2; do
    retry_dir="$REPORT_DIR/dogfood_retry_${_retry}"
    export EDITOR_PERF_DOGFOOD_OUT="$retry_dir"
    rm -rf "$retry_dir"
    mkdir -p "$retry_dir"
    set +e
    bash "$DOGFOOD" >/dev/null 2>"$REPORT_DIR/dogfood_retry_${_retry}.log"
    retry_status=$?
    set -e
    [ "$retry_status" -eq 0 ] || fail "dogfood scroll retry $_retry failed"
    retry_scroll="$(grep -E '^scroll_cpu_percent=' "$retry_dir/report.txt" | cut -d= -f2)"
    scroll_samples="$scroll_samples $retry_scroll"
  done
  scroll_cpu="$(printf '%s\n' $scroll_samples | sort -n | awk 'NR==2 { print; exit }')"
  echo "[editor_perf_startup_open] scroll samples: $scroll_samples → median=$scroll_cpu" >&2
fi
[ "$scroll_cpu" -le "$SCROLL_CPU_MAX" ] || fail "scroll_cpu=$scroll_cpu > $SCROLL_CPU_MAX"

# Prefer the OPEN-run present time for the report (load-bearing open path).
present_ms="$present_open"
ceiling_ms=$(( present_ms * 125 / 100 ))
if [ "$ceiling_ms" -gt "$TIME_MAX_MS" ]; then
  ceiling_ms="$TIME_MAX_MS"
fi
if [ "$ceiling_ms" -lt 1 ]; then
  ceiling_ms=1
fi

{
  echo "open_path=$open_path_open"
  echo "time_to_first_present_ms=$present_ms"
  echo "starter_open_path=$open_path_starter"
  echo "starter_time_to_first_present_ms=$present_starter"
  echo "time_to_first_present_ms_ceiling=$ceiling_ms"
  echo "scroll_cpu_percent=$scroll_cpu"
  echo "type_stall_ms=$stall_ms"
  echo "startup_full_wrap_deferred=1"
} > "$REPORT_DIR/report.txt"

echo "open_path=$open_path_open"
echo "time_to_first_present_ms=$present_ms"
echo "time_to_first_present_ms_ceiling=$ceiling_ms"
echo "scroll_cpu_percent=$scroll_cpu"
echo "type_stall_ms=$stall_ms"
echo "[editor_perf_startup_open] OK present=${present_ms}ms ceiling=${ceiling_ms}ms scroll=${scroll_cpu}% stall=${stall_ms}ms"
