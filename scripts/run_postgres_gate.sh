#!/usr/bin/env bash
# TRACK_STDLIB_POSTGRES STEP=4 — link smoke + stdlib codegen; live SELECT if env set.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "[postgres gate] runtime link smoke"
bash scripts/run_postgres_runtime_smoke.sh

echo "[postgres gate] MLC stdlib codegen"
bundle exec ruby -Ilib:test test/mlc/postgres_stdlib_test.rb

if [[ -n "${DATABASE_URL:-}" || -n "${PGHOST:-}" ]]; then
  echo "[postgres gate] live SELECT 1 expected via runtime smoke (env set)"
else
  echo "[postgres gate] live SELECT 1 skipped (set DATABASE_URL or PGHOST)"
fi

echo "[postgres gate] OK"
