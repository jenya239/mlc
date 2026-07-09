#!/usr/bin/env bash
# Multi-module --run gate (TRACK_MIR_VM_FULL Epic 4 STEP=11).
# Entry imports a sibling module; both lower into one MirProgram.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${VM_MULTI_ENTRY:-$ROOT_DIR/misc/examples/vm_multi/main.mlc}"
EXPECTED_EXIT="${VM_MULTI_EXPECTED_EXIT:-7}"

if [ ! -x "$MLCC" ]; then
  echo "[multi-module vm gate] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[multi-module vm gate] FAIL missing: $ENTRY" >&2
  exit 1
fi

TMPDIR="${TMPDIR:-/tmp}"
set +e
"$MLCC" --run "$ENTRY" >/dev/null 2>"$TMPDIR/mlcc_multi_module_$$.err"
status=$?
set -e

if grep -q '^error:' "$TMPDIR/mlcc_multi_module_$$.err" 2>/dev/null; then
  echo "[multi-module vm gate] FAIL $ENTRY (compiler error)" >&2
  head -5 "$TMPDIR/mlcc_multi_module_$$.err" >&2 || true
  rm -f "$TMPDIR/mlcc_multi_module_$$.err"
  exit 1
fi
rm -f "$TMPDIR/mlcc_multi_module_$$.err"

if [ "$status" -ne "$EXPECTED_EXIT" ]; then
  echo "[multi-module vm gate] FAIL $ENTRY exit=$status expected=$EXPECTED_EXIT" >&2
  exit 1
fi

echo "[multi-module vm gate] ok $ENTRY exit=$status" >&2
