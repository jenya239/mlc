#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103g — arrays/records unit.
# Not part of run_ux_gate. Green also runs dev_gate_fast separately.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/script_vm/tests/arrays_records_unit.mlc"
HEAP="$ROOT_DIR/script_vm/heap.mlc"
BYTECODE="$ROOT_DIR/script_vm/bytecode.mlc"
INTERPRETER="$ROOT_DIR/script_vm/interpreter.mlc"
RED="$ROOT_DIR/scripts/run_script_vm_arrays_records_unit_red.sh"
HEAP_UNIT="$ROOT_DIR/scripts/run_script_vm_heap_gc_arena_unit.sh"
CONTROL_UNIT="$ROOT_DIR/scripts/run_script_vm_control_flow_unit.sh"
OUT_DIR="${SCRIPT_VM_ARRAYS_RECORDS_OUT:-$ROOT_DIR/tmp/script_vm_arrays_records}"
BIN_OUT="$OUT_DIR/bin"
REPORT_DIR="${SCRIPT_VM_ARRAYS_RECORDS_REPORT:-$ROOT_DIR/.tmp/script_vm_arrays_records}"
REPORT_FILE="$REPORT_DIR/report.txt"

fail() {
  echo "[script_vm arrays_records_unit] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$HEAP" ] || fail "missing heap.mlc"
[ -f "$BYTECODE" ] || fail "missing bytecode.mlc"
[ -f "$INTERPRETER" ] || fail "missing interpreter.mlc"
[ -f "$RED" ] || fail "missing red harness"

grep -Eq 'heap_alloc_array|NEW_ARRAY|run_with_heap' "$HEAP" "$BYTECODE" "$INTERPRETER" || \
  fail "Array/Record / opcodes / run_with_heap missing"

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
printf '%s\n' "$output" | grep -q '\[script_vm\] arrays_records_unit ok' || \
  fail "missing arrays_records_unit ok"
printf '%s\n' "$output" | grep -q 'cycle_unrooted=ok' || fail "missing cycle_unrooted"
printf '%s\n' "$output" | grep -q 'cycle_rooted=ok' || fail "missing cycle_rooted"
printf '%s\n' "$output" | grep -q 'arrays_records=ok' || fail "missing arrays_records"
printf '%s\n' "$output" | grep -q 'write_barrier_hits=' || fail "missing write_barrier_hits"

if bash "$RED" >/tmp/script_vm_arrays_records_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -Eq 'already present|already declare' /tmp/script_vm_arrays_records_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/script_vm_arrays_records_red.out))"

# Side: §103f + §103e still ok
bash "$HEAP_UNIT" >/tmp/script_vm_arrays_records_heap_side.out 2>&1 || \
  fail "side heap_gc_arena unit failed"
bash "$CONTROL_UNIT" >/tmp/script_vm_arrays_records_cf_side.out 2>&1 || \
  fail "side control_flow unit failed"

{
  echo "arrays_records=ok"
  echo "cycle_unrooted=ok"
  echo "cycle_rooted=ok"
  echo "red_already_present=ok"
  echo "side_heap_gc_arena=ok"
  echo "side_control_flow=ok"
  printf '%s\n' "$output" | grep -E 'write_barrier_hits=' | tail -1
} | tee "$REPORT_FILE"

echo "[script_vm arrays_records_unit] ok" >&2
