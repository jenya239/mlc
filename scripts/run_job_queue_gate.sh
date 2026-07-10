#!/usr/bin/env bash
# TRACK_STDLIB_JOB_QUEUE STEP=4 — enqueue/delay/retry smoke (C++-only; no MLC module).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "[job_queue gate] runtime smoke (enqueue N + schedule_after + retry + shutdown)"
bash scripts/run_job_queue_runtime_smoke.sh

echo "[job_queue gate] OK (C++-only; no MLC stdlib test)"
