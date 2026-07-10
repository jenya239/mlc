#!/usr/bin/env bash
# TRACK_STDLIB_CRYPTO STEP=4 — link smoke (known-answer vectors) + stdlib codegen.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "[crypto gate] runtime link smoke (SHA-256/HMAC vectors)"
bash scripts/run_crypto_runtime_smoke.sh

echo "[crypto gate] MLC stdlib codegen"
bundle exec ruby -Ilib:test test/mlc/crypto_stdlib_test.rb

echo "[crypto gate] OK"
