#!/usr/bin/env bash
# Runtime concurrency smoke (TRACK_CONCURRENCY STEP=7).
# MLC_TSAN=1 adds -fsanitize=thread for stress_channel only.

set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
include="${root}/include"
test_directory="${root}/test"
source "${root}/../compiler/scripts/select_cxx.sh"
common_flags=(-std=c++20 -pthread -I"${include}")

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

tsan_flags=()
if [[ "${MLC_TSAN:-0}" == 1 ]]; then
  tsan_flags=(-fsanitize=thread -g)
  echo "[concurrency smoke] stress_channel (TSAN)"
else
  echo "[concurrency smoke] stress_channel"
fi
run_test stress_channel "${tsan_flags[@]}"

echo "[concurrency smoke] ok"
