#!/usr/bin/env bash
# TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE STEP=1 —
# Confirm Isolate MLC surface missing (E001) while C++ isolate.hpp exists.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/isolate_start_send.mlc"
ISOLATE_HPP="$ROOT_DIR/runtime/include/mlc/concurrency/isolate.hpp"

if [ ! -x "$MLCC" ]; then
  echo "[isolate_mlc_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if [ ! -f "$ISOLATE_HPP" ]; then
  echo "[isolate_mlc_red] FAIL: isolate.hpp missing (expected C++ already present)" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[isolate_mlc_red] FAIL: missing E001 (expected undefined Isolate)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'undefined: Isolate'; then
  echo "[isolate_mlc_red] FAIL: missing 'undefined: Isolate'" >&2
  exit 1
fi
if [ "$status" -eq 0 ]; then
  echo "[isolate_mlc_red] FAIL: check-only succeeded (expected red gap)" >&2
  exit 1
fi

echo "ok isolate_mlc_red"
echo "[isolate_mlc_red] ok (E001 Isolate, status=$status, isolate.hpp present)" >&2
