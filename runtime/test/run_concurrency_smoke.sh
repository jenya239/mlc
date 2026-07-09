#!/usr/bin/env bash
# Runtime concurrency smoke (TRACK_CONCURRENCY STEP=7 / HARNESS T1–T4).
# MLC_SANITIZE=address|undefined|thread — sanitizer flags on stress_* only.
# MLC_TSAN=1 — alias for MLC_SANITIZE=thread (compat).

set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
include="${root}/include"
test_directory="${root}/test"
source "${root}/../compiler/scripts/select_cxx.sh"
common_flags=(-std=c++20 -pthread -I"${include}")

if [[ -z "${MLC_SANITIZE:-}" && "${MLC_TSAN:-0}" == 1 ]]; then
  MLC_SANITIZE=thread
fi

sanitize_flags=()
case "${MLC_SANITIZE:-}" in
  "") ;;
  address) sanitize_flags=(-fsanitize=address -g -fno-omit-frame-pointer) ;;
  undefined) sanitize_flags=(-fsanitize=undefined -g) ;;
  thread) sanitize_flags=(-fsanitize=thread -g) ;;
  *)
    echo "unknown MLC_SANITIZE=${MLC_SANITIZE} (want address|undefined|thread)" >&2
    exit 1
    ;;
esac

run_test() {
  local source_name="$1"
  local extra_flags=("${@:2}")
  local binary="${test_directory}/${source_name}"
  "${CXX_CMD[@]}" "${common_flags[@]}" "${extra_flags[@]}" \
    -o "${binary}" "${test_directory}/${source_name}.cpp"
  "${binary}"
}

echo "[concurrency smoke] test_cow_detach"
run_test test_cow_detach
echo "[concurrency smoke] test_channel"
run_test test_channel
echo "[concurrency smoke] test_spawn"
run_test test_spawn
echo "[concurrency smoke] test_arc"
run_test test_arc
echo "[concurrency smoke] test_mutex"
run_test test_mutex
echo "[concurrency smoke] test_scheduler"
run_test test_scheduler
echo "[concurrency smoke] test_stop"
run_test test_stop

if [[ -n "${MLC_SANITIZE:-}" ]]; then
  echo "[concurrency smoke] stress_channel (${MLC_SANITIZE})"
else
  echo "[concurrency smoke] stress_channel"
fi
run_test stress_channel "${sanitize_flags[@]}"
echo "[concurrency smoke] stress_mutex"
run_test stress_mutex "${sanitize_flags[@]}"
echo "[concurrency smoke] stress_arc"
run_test stress_arc "${sanitize_flags[@]}"
echo "[concurrency smoke] stress_spawn"
run_test stress_spawn "${sanitize_flags[@]}"

echo "[concurrency smoke] ok"
