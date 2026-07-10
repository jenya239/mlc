#!/usr/bin/env bash
# TRACK_STDLIB_ENV_LOGGING STEP=4 — env/log runtime smoke + stdlib codegen.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "[env_log gate] runtime smoke (getenv + JSON lines)"
bash scripts/run_env_log_runtime_smoke.sh

echo "[env_log gate] MLC stdlib codegen"
bundle exec ruby -Ilib:test test/mlc/env_log_stdlib_test.rb

echo "[env_log gate] OK"
