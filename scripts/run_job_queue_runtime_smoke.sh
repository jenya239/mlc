#!/usr/bin/env bash
# Compile+run JobQueue runtime smoke (TRACK_STDLIB_JOB_QUEUE STEP=2).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INCLUDE="$ROOT/runtime/include"
SOURCE="$ROOT/runtime/test/test_job_queue.cpp"
OUT="$ROOT/runtime/test/test_job_queue"
CXX="${CXX:-g++}"

"$CXX" -std=c++20 -pthread "-I${INCLUDE}" -o "$OUT" "$SOURCE"
"$OUT"
echo "job_queue runtime smoke OK"
