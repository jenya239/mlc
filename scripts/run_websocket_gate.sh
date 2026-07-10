#!/usr/bin/env bash
# TRACK_STDLIB_WEBSOCKET STEP=4 — upgrade+text echo smoke + stdlib codegen.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "[websocket gate] runtime smoke (RFC Accept + text echo)"
bash scripts/run_websocket_runtime_smoke.sh

echo "[websocket gate] MLC stdlib codegen"
bundle exec ruby -Ilib:test test/mlc/websocket_stdlib_test.rb

echo "[websocket gate] OK"
