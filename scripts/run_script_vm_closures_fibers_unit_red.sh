#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103h — Red: no closures/fibers unit / green runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_closures_fibers_unit.sh
# + script_vm/tests/closures_fibers_unit.mlc (+ Closure + CALL/MAKE_CLOSURE/upvals + run_program).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNIT="$ROOT_DIR/script_vm/tests/closures_fibers_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_closures_fibers_unit.sh"
HEAP="$ROOT_DIR/script_vm/heap.mlc"
BYTECODE="$ROOT_DIR/script_vm/bytecode.mlc"
INTERPRETER="$ROOT_DIR/script_vm/interpreter.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_MLC_SCRIPT_VM.md"
ARRAYS_UNIT="$ROOT_DIR/scripts/run_script_vm_arrays_records_unit.sh"

fail() {
  echo "[script_vm closures_fibers_unit_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_MLC_SCRIPT_VM.md (unexpected drift)"
[ -f "$HEAP" ] || fail "missing heap.mlc (unexpected drift — §103f required)"
[ -f "$BYTECODE" ] || fail "missing bytecode.mlc (unexpected drift — §103b required)"
[ -f "$INTERPRETER" ] || fail "missing interpreter.mlc (unexpected drift — §103d required)"
[ -f "$ARRAYS_UNIT" ] || fail "missing arrays_records unit (unexpected drift — §103g required)"

# Red: green runner / unit absent until Green.
if [ -f "$GREEN" ]; then
  fail "green closures_fibers unit runner already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "closures_fibers_unit.mlc already present (expected gap until Green)"
fi

# Red: Closure / call / upval / run_program absent until Green.
if grep -Eq 'heap_alloc_closure|heap_type_closure|MAKE_CLOSURE|GET_UPVAL|SET_UPVAL|opcode_call|instruction_opcode_call|opcode_make_closure|opcode_get_upval|opcode_set_upval|run_program|FunctionProto' \
  "$HEAP" "$BYTECODE" "$INTERPRETER" 2>/dev/null; then
  fail "sources already declare Closure / CALL/upvals / run_program (expected gap until Green)"
fi

# Red: no §103h Green measured paste yet.
if grep -A40 '### §103h Green measured\|Green measured (§103h)\|### Green measured (§103h)' "$TRACK" 2>/dev/null | \
  grep -Eq 'closures_fibers=ok|upvalue_mut=ok|recursion=ok'; then
  fail "TRACK already has closures_fibers post-cut measured counters (expected gap until Green)"
fi

fail "no script_vm closures_fibers unit (Red expected)"
