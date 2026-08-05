#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110a — Green: Xvfb isolate for VISIBLE=1 perf.
# Opt-in: MLC_EDITOR_PERF_XVFB=1 → require xvfb-run/Xvfb, never fall back to :0.
# Unset/0 → run child on ambient DISPLAY (no forced Xvfb).
# Default child: run_editor_perf_dogfood_gate.sh. Override: -- <argv...>
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
REPORT_DIR="${EDITOR_PERF_XVFB_OUT:-$ROOT_DIR/.tmp/editor_perf_xvfb}"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
GATE_OUT="${EDITOR_PERF_DOGFOOD_GATE_OUT:-$REPORT_DIR/dogfood_gate}"
XVFB_FLAG="${MLC_EDITOR_PERF_XVFB:-0}"

fail() {
  echo "[editor_perf_xvfb] FAIL: $1" >&2
  exit 1
}

[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (§109k)"

CHILD_LABEL="dogfood_gate"
if [ "${1:-}" = "--" ]; then
  shift
  [ "$#" -ge 1 ] || fail "empty argv after --"
  CHILD_LABEL="custom"
  CHILD_SCRIPT=""
  CHILD_ARGS=("$@")
else
  CHILD_SCRIPT="$DOGFOOD_GATE"
  CHILD_ARGS=()
fi

mkdir -p "$REPORT_DIR" "$GATE_OUT"
REPORT_FILE="$REPORT_DIR/report.txt"
CHILD_LOG="$REPORT_DIR/child.log"
DISPLAY_FILE="$REPORT_DIR/display.txt"
: >"$REPORT_FILE"
: >"$CHILD_LOG"
: >"$DISPLAY_FILE"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
export MLC_GLFW_VISIBLE=1
export EDITOR_PERF_DOGFOOD_GATE_OUT="$GATE_OUT"
# Propagate isolate flag so children / future present-pacing can detect Xvfb runs.
if [ "$XVFB_FLAG" = "1" ]; then
  export MLC_EDITOR_PERF_XVFB=1
fi
unset MLC_EDITOR_PERF || true

require_xvfb_tools() {
  local xvfb_run_path xvfb_path
  xvfb_run_path="$(command -v xvfb-run 2>/dev/null || true)"
  # Real Debian/Ubuntu xvfb-run is a shell script that mentions Xvfb.
  if [ -n "$xvfb_run_path" ] && grep -q 'Xvfb' "$xvfb_run_path" 2>/dev/null; then
    echo "xvfb-run"
    return 0
  fi
  xvfb_path="$(command -v Xvfb 2>/dev/null || true)"
  if [ -n "$xvfb_path" ] && [ -x "$xvfb_path" ] && \
      file "$xvfb_path" 2>/dev/null | grep -q 'ELF'; then
    echo "Xvfb"
    return 0
  fi
  fail "Xvfb/xvfb-run not on PATH — install package xvfb (e.g. apt-get install -y xvfb); skip-green forbidden"
}

pick_display_number() {
  local number=99
  while [ "$number" -lt 200 ]; do
    if [ ! -e "/tmp/.X${number}-lock" ] && [ ! -S "/tmp/.X11-unix/X${number}" ]; then
      echo "$number"
      return 0
    fi
    number=$((number + 1))
  done
  fail "no free X display number in 99..199"
}

assert_not_display_zero() {
  local display_name="$1"
  case "$display_name" in
    :0|:0.*) fail "DISPLAY still $display_name under Xvfb — isolate broken" ;;
  esac
}

run_child() {
  if [ -n "${CHILD_SCRIPT:-}" ]; then
    bash "$CHILD_SCRIPT"
  else
    "${CHILD_ARGS[@]}"
  fi
}

run_with_manual_xvfb() {
  local display_number display_name xvfb_pid child_status
  display_number="$(pick_display_number)"
  display_name=":${display_number}"
  Xvfb "$display_name" -screen 0 1280x800x24 -nolisten tcp >/dev/null 2>&1 &
  xvfb_pid=$!
  cleanup_xvfb() {
    kill "$xvfb_pid" 2>/dev/null || true
    wait "$xvfb_pid" 2>/dev/null || true
  }
  trap cleanup_xvfb EXIT
  sleep 0.3
  if ! kill -0 "$xvfb_pid" 2>/dev/null; then
    fail "Xvfb failed to start on $display_name"
  fi
  export DISPLAY="$display_name"
  assert_not_display_zero "$DISPLAY"
  printf 'display=%s\n' "$DISPLAY" >"$DISPLAY_FILE"
  set +e
  run_child >"$CHILD_LOG" 2>&1
  child_status=$?
  set -e
  cleanup_xvfb
  trap - EXIT
  return "$child_status"
}

run_with_xvfb_run() {
  local child_status
  set +e
  if [ -n "${CHILD_SCRIPT:-}" ]; then
    xvfb-run -a -s "-screen 0 1280x800x24" \
      env MLC_GLFW_VISIBLE=1 \
          MLC_EDITOR_PERF_XVFB=1 \
          MLC_EDITOR_ROOT="$MLC_EDITOR_ROOT" \
          TMPDIR="$TMPDIR" \
          EDITOR_PERF_DOGFOOD_GATE_OUT="$GATE_OUT" \
          EDITOR_PERF_XVFB_DISPLAY_FILE="$DISPLAY_FILE" \
      bash -c '
        set -euo pipefail
        printf "display=%s\n" "${DISPLAY:-}" >"$EDITOR_PERF_XVFB_DISPLAY_FILE"
        case "${DISPLAY:-}" in
          :0|:0.*)
            echo "[editor_perf_xvfb] FAIL: DISPLAY still $DISPLAY under xvfb-run — isolate broken" >&2
            exit 1
            ;;
        esac
        unset MLC_EDITOR_PERF || true
        exec bash "'"$CHILD_SCRIPT"'"
      ' >"$CHILD_LOG" 2>&1
  else
    xvfb-run -a -s "-screen 0 1280x800x24" \
      env MLC_GLFW_VISIBLE=1 \
          MLC_EDITOR_PERF_XVFB=1 \
          MLC_EDITOR_ROOT="$MLC_EDITOR_ROOT" \
          TMPDIR="$TMPDIR" \
          EDITOR_PERF_DOGFOOD_GATE_OUT="$GATE_OUT" \
          EDITOR_PERF_XVFB_DISPLAY_FILE="$DISPLAY_FILE" \
      bash -c '
        set -euo pipefail
        printf "display=%s\n" "${DISPLAY:-}" >"$EDITOR_PERF_XVFB_DISPLAY_FILE"
        case "${DISPLAY:-}" in
          :0|:0.*)
            echo "[editor_perf_xvfb] FAIL: DISPLAY still $DISPLAY under xvfb-run — isolate broken" >&2
            exit 1
            ;;
        esac
        unset MLC_EDITOR_PERF || true
        exec "$@"
      ' unused "${CHILD_ARGS[@]}" >"$CHILD_LOG" 2>&1
  fi
  child_status=$?
  set -e
  return "$child_status"
}

CHILD_EXIT=0
USED_XVFB=0
DISPLAY_REPORTED="${DISPLAY:-}"
TOOL=""

if [ "$XVFB_FLAG" = "1" ]; then
  TOOL="$(require_xvfb_tools)"
  USED_XVFB=1
  if [ "$TOOL" = "xvfb-run" ]; then
    set +e
    run_with_xvfb_run
    CHILD_EXIT=$?
    set -e
  else
    set +e
    run_with_manual_xvfb
    CHILD_EXIT=$?
    set -e
  fi
  if [ -f "$DISPLAY_FILE" ]; then
    DISPLAY_REPORTED="$(grep -E '^display=' "$DISPLAY_FILE" | head -n1 | cut -d= -f2-)"
  fi
else
  printf 'display=%s\n' "${DISPLAY:-}" >"$DISPLAY_FILE"
  set +e
  run_child >"$CHILD_LOG" 2>&1
  CHILD_EXIT=$?
  set -e
  DISPLAY_REPORTED="${DISPLAY:-}"
fi

{
  echo "xvfb=$USED_XVFB"
  echo "display=${DISPLAY_REPORTED:-}"
  echo "child=$CHILD_LABEL"
  echo "child_exit=$CHILD_EXIT"
  if [ -f "$GATE_OUT/report.txt" ]; then
    grep -E '^(scroll_cpu_percent|type_stall_ms|member=)' "$GATE_OUT/report.txt" || true
  fi
} >"$REPORT_FILE"

if [ "$CHILD_EXIT" -ne 0 ]; then
  echo "[editor_perf_xvfb] child failed exit=$CHILD_EXIT — see $CHILD_LOG" >&2
  tail -n 40 "$CHILD_LOG" >&2 || true
  exit "$CHILD_EXIT"
fi

echo "[editor_perf_xvfb] OK xvfb=$USED_XVFB display=${DISPLAY_REPORTED:-} child=$CHILD_LABEL" >&2
cat "$REPORT_FILE"
