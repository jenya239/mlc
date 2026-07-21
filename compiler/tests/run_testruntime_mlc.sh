#!/usr/bin/env bash
# TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE STEP=2 —
# check-only + link/run new/spawn/join; same-seed e2e; C++ test_scheduler.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/testruntime_new_spawn.mlc"
SEED_FIXTURE="$COMPILER_DIR/tests/fixtures/testruntime_same_seed.mlc"
OUT_DIR="${TMPDIR:-/tmp}/mlc_testruntime_mlc_$$"
BIN_OUT="$OUT_DIR/testruntime_new_spawn"
TEST_SCHEDULER="$ROOT_DIR/runtime/test/test_scheduler"

if [ ! -x "$MLCC" ]; then
  echo "[testruntime_mlc] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if [ "$status" -ne 0 ]; then
  echo "[testruntime_mlc] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[testruntime_mlc] FAIL: still E001 (expected green after STEP=2)" >&2
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
  echo "[testruntime_mlc] FAIL: linked fixture exit=$run_status" >&2
  exit 1
fi

SEED_OUT="$OUT_DIR/seed"
mkdir -p "$SEED_OUT"
"$MLCC" -o "$SEED_OUT" "$SEED_FIXTURE"
"$COMPILER_DIR/build_bin.sh" "$SEED_OUT" "$SEED_OUT/prog"
set +e
"$SEED_OUT/prog"
seed_status=$?
set -e
if [ "$seed_status" -ne 0 ]; then
  echo "[testruntime_mlc] FAIL: same-seed e2e exit=$seed_status" >&2
  exit 1
fi

if [ ! -x "$TEST_SCHEDULER" ]; then
  (cd "$ROOT_DIR/runtime/test" && ./run_concurrency_smoke.sh) >/dev/null
fi
set +e
"$TEST_SCHEDULER" >/dev/null
scheduler_status=$?
set -e
if [ "$scheduler_status" -ne 0 ]; then
  echo "[testruntime_mlc] FAIL: test_scheduler exit=$scheduler_status" >&2
  exit 1
fi

echo "ok testruntime_mlc"
echo "[testruntime_mlc] ok (check-only + link + same-seed + test_scheduler)" >&2
