#!/usr/bin/env bash
# TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED STEP=1 —
# Confirm make_unbounded_channel is undefined (E001).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/channel_unbounded.mlc"
CHANNEL_HPP="$ROOT_DIR/runtime/include/mlc/concurrency/channel.hpp"

if [ ! -x "$MLCC" ]; then
  echo "[channel_unbounded_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -qE 'make_unbounded_channel|unbounded_channel' "$CHANNEL_HPP"; then
  echo "[channel_unbounded_red] FAIL: unbounded already in channel.hpp (expected red gap)" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[channel_unbounded_red] FAIL: missing E001 (expected undefined make_unbounded_channel)" >&2
  exit 1
fi
if [ "$status" -eq 0 ]; then
  echo "[channel_unbounded_red] FAIL: check-only succeeded (expected red gap)" >&2
  exit 1
fi

echo "ok channel_unbounded_red"
echo "[channel_unbounded_red] ok (E001 present, status=$status)" >&2
