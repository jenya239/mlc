#!/usr/bin/env bash
# TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE STEP=1 —
# Confirm Supervisor MLC surface missing (E001) while C++ supervisor.hpp exists.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/supervisor_add_start.mlc"
SUPERVISOR_HPP="$ROOT_DIR/runtime/include/mlc/concurrency/supervisor.hpp"

if [ ! -x "$MLCC" ]; then
  echo "[supervisor_mlc_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if [ ! -f "$SUPERVISOR_HPP" ]; then
  echo "[supervisor_mlc_red] FAIL: supervisor.hpp missing (expected C++ already present)" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if ! printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[supervisor_mlc_red] FAIL: missing E001 (expected undefined Supervisor)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'undefined: Supervisor'; then
  echo "[supervisor_mlc_red] FAIL: missing 'undefined: Supervisor'" >&2
  exit 1
fi
if [ "$status" -eq 0 ]; then
  echo "[supervisor_mlc_red] FAIL: check-only succeeded (expected red gap)" >&2
  exit 1
fi

echo "ok supervisor_mlc_red"
echo "[supervisor_mlc_red] ok (E001 Supervisor, status=$status, supervisor.hpp present)" >&2
