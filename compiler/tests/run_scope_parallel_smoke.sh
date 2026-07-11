#!/usr/bin/env bash
# TRACK_CONCURRENCY_SPAWN_DETACH STEP=3 — scope |s| spawn wall-time < serial sleeps.
set -euo pipefail

root="$(cd "$(dirname "$0")/../.." && pwd)"
mlcc="${MLCC:-$root/compiler/out/mlcc}"
source "$root/compiler/scripts/select_cxx.sh"
runtime_include="$root/runtime/include"
runtime_sources=("$root/runtime/src/io/io.cpp" "$root/runtime/src/core/string.cpp")
parallel_fixture="$root/compiler/tests/e2e/scope_parallel_sleep.mlc"
serial_fixture="$root/compiler/tests/e2e/scope_serial_sleep.mlc"
workdir="$(mktemp -d "${TMPDIR:-/tmp}/scope_parallel.XXXXXX")"
trap 'rm -rf "$workdir"' EXIT

if [[ ! -x "$mlcc" ]]; then
  echo "[scope parallel] FAIL: mlcc not executable at $mlcc" >&2
  exit 1
fi

elapsed_ms() {
  local binary="$1"
  local start_ns end_ns
  start_ns="$(date +%s%N)"
  "$binary" >/dev/null
  end_ns="$(date +%s%N)"
  echo $(( (end_ns - start_ns) / 1000000 ))
}

build_program() {
  local fixture="$1"
  local out_dir="$2"
  local binary="$3"
  mkdir -p "$out_dir"
  "$mlcc" -o "$out_dir" "$fixture"
  "${CXX_CMD[@]}" -std=c++20 -pthread \
    -I "$out_dir" -I "$runtime_include" \
    "$out_dir"/*.cpp "${runtime_sources[@]}" \
    -o "$binary"
}

echo "[scope parallel] mlcc=$mlcc" >&2
build_program "$parallel_fixture" "$workdir/parallel_out" "$workdir/parallel"
build_program "$serial_fixture" "$workdir/serial_out" "$workdir/serial"

if ! grep -q 'TaskScope' "$workdir/parallel_out"/*.cpp; then
  echo "[scope parallel] FAIL: generated C++ missing TaskScope" >&2
  exit 1
fi
if ! grep -q '\.spawn(' "$workdir/parallel_out"/*.cpp; then
  echo "[scope parallel] FAIL: generated C++ missing .spawn(" >&2
  exit 1
fi

parallel_ms="$(elapsed_ms "$workdir/parallel")"
serial_ms="$(elapsed_ms "$workdir/serial")"
echo "[scope parallel] parallel_ms=${parallel_ms} serial_ms=${serial_ms}" >&2

# Two 250ms sleeps: serial ≥ ~500ms; parallel should stay near one sleep.
if [[ "$serial_ms" -lt 400 ]]; then
  echo "[scope parallel] FAIL: serial too fast (${serial_ms}ms < 400ms) — sleep broken?" >&2
  exit 1
fi
if [[ "$parallel_ms" -ge "$serial_ms" ]]; then
  echo "[scope parallel] FAIL: parallel (${parallel_ms}ms) not faster than serial (${serial_ms}ms)" >&2
  exit 1
fi
# Parallel must be clearly sub-serial (allow OS jitter): < 75% of serial.
threshold_ms=$(( serial_ms * 75 / 100 ))
if [[ "$parallel_ms" -ge "$threshold_ms" ]]; then
  echo "[scope parallel] FAIL: parallel (${parallel_ms}ms) >= 75% of serial (${threshold_ms}ms)" >&2
  exit 1
fi

echo "[scope parallel] PASS (parallel ${parallel_ms}ms < serial ${serial_ms}ms)"
