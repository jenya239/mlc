#!/usr/bin/env bash
# TRACK_VM_TRAMPOLINE STEP=3: deep MIR-step fixture under default C++ stack.
# Must complete without segfault and without `ulimit -s unlimited`.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
PROGRAM="${VM_DEEP_LOOP_PROGRAM:-$ROOT_DIR/misc/examples/vm_deep_loop.mlc}"
EXPECTED_EXIT="${VM_DEEP_LOOP_EXIT:-7}"

if [ ! -x "$MLCC" ]; then
  echo "[vm trampoline depth] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$PROGRAM" ]; then
  echo "[vm trampoline depth] FAIL missing: $PROGRAM" >&2
  exit 1
fi

# Explicitly keep default stack — raising it would hide the old bug class.
stack_limit="$(ulimit -s)"
if [ "$stack_limit" = "unlimited" ]; then
  echo "[vm trampoline depth] WARN: ulimit -s is unlimited; restoring soft default 8192" >&2
  ulimit -s 8192
fi

TMPDIR="${TMPDIR:-/tmp}"
out_file="$TMPDIR/mlcc_vm_deep_$$.out"
err_file="$TMPDIR/mlcc_vm_deep_$$.err"

set +e
"$MLCC" --run "$PROGRAM" >"$out_file" 2>"$err_file"
status=$?
set -e

if grep -qE '^error:' "$out_file" "$err_file" 2>/dev/null; then
  echo "[vm trampoline depth] FAIL $PROGRAM (compiler error, exit=$status)" >&2
  grep -hE '^error:' "$out_file" "$err_file" 2>/dev/null | head -3 >&2 || true
  rm -f "$out_file" "$err_file"
  exit 1
fi

# Segfault / abort typically exit 139 / 134.
if [ "$status" -ge 128 ]; then
  echo "[vm trampoline depth] FAIL $PROGRAM crashed (exit=$status, stack_limit=$(ulimit -s))" >&2
  rm -f "$out_file" "$err_file"
  exit 1
fi

if [ "$status" -ne "$EXPECTED_EXIT" ]; then
  echo "[vm trampoline depth] FAIL $PROGRAM exit=$status expected=$EXPECTED_EXIT" >&2
  rm -f "$out_file" "$err_file"
  exit 1
fi

rm -f "$out_file" "$err_file"
echo "[vm trampoline depth] ok $PROGRAM exit=$status (ulimit -s=$(ulimit -s))" >&2
