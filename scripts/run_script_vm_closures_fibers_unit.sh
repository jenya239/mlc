#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103h — closures/fibers (call frames) unit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/script_vm/tests/closures_fibers_unit.mlc"
HEAP="$ROOT_DIR/script_vm/heap.mlc"
BYTECODE="$ROOT_DIR/script_vm/bytecode.mlc"
INTERPRETER="$ROOT_DIR/script_vm/interpreter.mlc"
RED="$ROOT_DIR/scripts/run_script_vm_closures_fibers_unit_red.sh"
ARRAYS_UNIT="$ROOT_DIR/scripts/run_script_vm_arrays_records_unit.sh"
CONTROL_UNIT="$ROOT_DIR/scripts/run_script_vm_control_flow_unit.sh"
OUT_DIR="${SCRIPT_VM_CLOSURES_FIBERS_OUT:-$ROOT_DIR/tmp/script_vm_closures_fibers}"
BIN_OUT="$OUT_DIR/bin"
REPORT_DIR="${SCRIPT_VM_CLOSURES_FIBERS_REPORT:-$ROOT_DIR/.tmp/script_vm_closures_fibers}"
REPORT_FILE="$REPORT_DIR/report.txt"

fail() {
  echo "[script_vm closures_fibers_unit] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$HEAP" ] || fail "missing heap.mlc"
[ -f "$BYTECODE" ] || fail "missing bytecode.mlc"
[ -f "$INTERPRETER" ] || fail "missing interpreter.mlc"
[ -f "$RED" ] || fail "missing red harness"

grep -Eq 'heap_alloc_closure|opcode_call|run_program|MAKE_CLOSURE|GET_UPVAL' \
  "$HEAP" "$BYTECODE" "$INTERPRETER" || fail "Closure/CALL/run_program missing"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR" "$REPORT_DIR"
mkdir -p "$OUT_DIR" "$REPORT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
[ -x "$BIN_OUT" ] || fail "missing binary"

set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  fail "unit exit=$status"
fi
printf '%s\n' "$output" | grep -q '\[script_vm\] closures_fibers_unit ok' || \
  fail "missing closures_fibers_unit ok"
printf '%s\n' "$output" | grep -q 'upvalue_mut=ok' || fail "missing upvalue_mut"
printf '%s\n' "$output" | grep -q 'recursion=ok' || fail "missing recursion"
printf '%s\n' "$output" | grep -q 'closures_fibers=ok' || fail "missing closures_fibers"

if bash "$RED" >/tmp/script_vm_closures_fibers_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -Eq 'already present|already declare' /tmp/script_vm_closures_fibers_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/script_vm_closures_fibers_red.out))"

bash "$ARRAYS_UNIT" >/tmp/script_vm_closures_fibers_arrays_side.out 2>&1 || \
  fail "side arrays_records unit failed"
bash "$CONTROL_UNIT" >/tmp/script_vm_closures_fibers_cf_side.out 2>&1 || \
  fail "side control_flow unit failed"

{
  echo "closures_fibers=ok"
  echo "upvalue_mut=ok"
  echo "recursion=ok"
  echo "red_already_present=ok"
  echo "side_arrays_records=ok"
  echo "side_control_flow=ok"
  printf '%s\n' "$output" | grep -E 'write_barrier_hits=' | tail -1
} | tee "$REPORT_FILE"

echo "[script_vm closures_fibers_unit] ok" >&2
