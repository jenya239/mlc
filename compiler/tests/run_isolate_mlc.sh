#!/usr/bin/env bash
# TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE STEP=2 —
# check-only + link/run start/send/shutdown; C++ test_isolate for concurrent serial.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/isolate_start_send.mlc"
SERIAL_FIXTURE="$COMPILER_DIR/tests/fixtures/isolate_serial_sends.mlc"
OUT_DIR="${TMPDIR:-/tmp}/mlc_isolate_mlc_$$"
BIN_OUT="$OUT_DIR/isolate_start_send"
TEST_ISOLATE="$ROOT_DIR/runtime/test/test_isolate"

if [ ! -x "$MLCC" ]; then
  echo "[isolate_mlc] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

set +e
output=$("$MLCC" --check-only "$FIXTURE" 2>&1)
status=$?
set -e
printf '%s\n' "$output"

if [ "$status" -ne 0 ]; then
  echo "[isolate_mlc] FAIL: check-only status=$status (expected 0 after STEP=2)" >&2
  exit 1
fi
if printf '%s\n' "$output" | grep -q 'error\[E001\]'; then
  echo "[isolate_mlc] FAIL: still E001 (expected green after STEP=2)" >&2
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
  echo "[isolate_mlc] FAIL: linked fixture exit=$run_status" >&2
  exit 1
fi

SERIAL_OUT="$OUT_DIR/serial"
mkdir -p "$SERIAL_OUT"
"$MLCC" -o "$SERIAL_OUT" "$SERIAL_FIXTURE"
"$COMPILER_DIR/build_bin.sh" "$SERIAL_OUT" "$SERIAL_OUT/prog"
set +e
"$SERIAL_OUT/prog"
serial_status=$?
set -e
if [ "$serial_status" -ne 0 ]; then
  echo "[isolate_mlc] FAIL: serial sends exit=$serial_status" >&2
  exit 1
fi

if [ ! -x "$TEST_ISOLATE" ]; then
  (cd "$ROOT_DIR/runtime/test" && ./run_concurrency_smoke.sh) >/dev/null
fi
set +e
"$TEST_ISOLATE" >/dev/null
isolate_status=$?
set -e
if [ "$isolate_status" -ne 0 ]; then
  echo "[isolate_mlc] FAIL: test_isolate exit=$isolate_status (concurrent serial gate)" >&2
  exit 1
fi

echo "ok isolate_mlc"
echo "[isolate_mlc] ok (check-only + link + serial + test_isolate)" >&2
