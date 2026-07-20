#!/usr/bin/env bash
# TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE STEP=2 —
# check-only + link/run add/start/stop; restart e2e; C++ test_supervisor.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/supervisor_add_start.mlc"
RESTART_FIXTURE="$COMPILER_DIR/tests/fixtures/supervisor_restart_permanent.mlc"
OUT_DIR="${TMPDIR:-/tmp}/mlc_supervisor_mlc_$$"
BIN_OUT="$OUT_DIR/supervisor_add_start"
TEST_SUPERVISOR="$ROOT_DIR/runtime/test/test_supervisor"

if [ ! -x "$MLCC" ]; then
  echo "[supervisor_mlc] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if [ "$status" -ne 0 ]; then
  echo "[supervisor_mlc] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[supervisor_mlc] FAIL: still E001 (expected green after STEP=2)" >&2
  exit 1
fi

mkdir -p "$OUT_DIR"
cleanup() { rm -rf "$OUT_DIR"; }
trap cleanup EXIT

"$MLCC" -o "$OUT_DIR" "$FIXTURE"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
run_status=$?
set -e
if [ "$run_status" -ne 0 ]; then
  echo "[supervisor_mlc] FAIL: linked fixture exit=$run_status" >&2
  exit 1
fi

RESTART_OUT="$OUT_DIR/restart"
mkdir -p "$RESTART_OUT"
"$MLCC" -o "$RESTART_OUT" "$RESTART_FIXTURE"
"$COMPILER_DIR/build_bin.sh" "$RESTART_OUT" "$RESTART_OUT/prog"
set +e
"$RESTART_OUT/prog"
restart_status=$?
set -e
if [ "$restart_status" -ne 0 ]; then
  echo "[supervisor_mlc] FAIL: restart e2e exit=$restart_status" >&2
  exit 1
fi

if [ ! -x "$TEST_SUPERVISOR" ]; then
  (cd "$ROOT_DIR/runtime/test" && ./run_concurrency_smoke.sh) >/dev/null
fi
set +e
"$TEST_SUPERVISOR" >/dev/null
supervisor_status=$?
set -e
if [ "$supervisor_status" -ne 0 ]; then
  echo "[supervisor_mlc] FAIL: test_supervisor exit=$supervisor_status" >&2
  exit 1
fi

echo "ok supervisor_mlc"
echo "[supervisor_mlc] ok (check-only + link + restart + test_supervisor)" >&2
