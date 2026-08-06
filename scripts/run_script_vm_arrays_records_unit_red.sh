#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103g — Red: no arrays/records unit / green runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_arrays_records_unit.sh
# + script_vm/tests/arrays_records_unit.mlc (+ Array/Record heap + opcodes 16–21).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNIT="$ROOT_DIR/script_vm/tests/arrays_records_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_arrays_records_unit.sh"
HEAP="$ROOT_DIR/script_vm/heap.mlc"
BYTECODE="$ROOT_DIR/script_vm/bytecode.mlc"
INTERPRETER="$ROOT_DIR/script_vm/interpreter.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_MLC_SCRIPT_VM.md"
HEAP_UNIT="$ROOT_DIR/scripts/run_script_vm_heap_gc_arena_unit.sh"

fail() {
  echo "[script_vm arrays_records_unit_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_MLC_SCRIPT_VM.md (unexpected drift)"
[ -f "$HEAP" ] || fail "missing heap.mlc (unexpected drift — §103f required)"
[ -f "$BYTECODE" ] || fail "missing bytecode.mlc (unexpected drift — §103b required)"
[ -f "$INTERPRETER" ] || fail "missing interpreter.mlc (unexpected drift — §103d required)"
[ -f "$HEAP_UNIT" ] || fail "missing heap_gc_arena unit (unexpected drift — §103f required)"

# Red: green runner / unit absent until Green.
if [ -f "$GREEN" ]; then
  fail "green arrays_records unit runner already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "arrays_records_unit.mlc already present (expected gap until Green)"
fi

# Red: Array/Record alloc + heap opcodes + run_with_heap absent until Green.
if grep -Eq 'heap_alloc_array|heap_alloc_record|NEW_ARRAY|GET_INDEX|SET_INDEX|NEW_RECORD|GET_PROP|SET_PROP|run_with_heap|opcode_new_array|instruction_opcode_new_array' \
  "$HEAP" "$BYTECODE" "$INTERPRETER" 2>/dev/null; then
  fail "sources already declare Array/Record / opcodes 16–21 / run_with_heap (expected gap until Green)"
fi

# Red: no §103g Green measured paste yet.
if grep -A40 '### §103g Green measured\|Green measured (§103g)\|### Green measured (§103g)' "$TRACK" 2>/dev/null | \
  grep -Eq 'arrays_records=ok|cycle_unrooted=ok|cycle_rooted=ok'; then
  fail "TRACK already has arrays_records post-cut measured counters (expected gap until Green)"
fi

fail "no script_vm arrays_records unit (Red expected)"
