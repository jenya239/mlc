#!/usr/bin/env bash
# TRACK_STDLIB_VALIDATION STEP=4 — valid/invalid runtime smoke + stdlib codegen.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "[validate gate] runtime smoke (valid/invalid fixtures)"
bash scripts/run_validate_runtime_smoke.sh

echo "[validate gate] MLC stdlib codegen"
bundle exec ruby -Ilib:test test/mlc/validate_stdlib_test.rb

echo "[validate gate] OK"
