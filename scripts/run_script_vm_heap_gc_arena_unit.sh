#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103f — heap GC arena unit.
# Not part of run_ux_gate this sub-track. Green also runs dev_gate_fast separately.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/script_vm/tests/heap_gc_arena_unit.mlc"
HEAP="$ROOT_DIR/script_vm/heap.mlc"
VALUE="$ROOT_DIR/script_vm/value.mlc"
RED="$ROOT_DIR/scripts/run_script_vm_heap_gc_arena_unit_red.sh"
OUT_DIR="${SCRIPT_VM_HEAP_GC_ARENA_OUT:-$ROOT_DIR/tmp/script_vm_heap_gc_arena}"
BIN_OUT="$OUT_DIR/bin"
REPORT_DIR="${SCRIPT_VM_HEAP_GC_ARENA_REPORT:-$ROOT_DIR/.tmp/script_vm_heap_gc_arena}"
REPORT_FILE="$REPORT_DIR/report.txt"

fail() {
  echo "[script_vm heap_gc_arena_unit] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$HEAP" ] || fail "missing heap.mlc"
[ -f "$VALUE" ] || fail "missing value.mlc"
[ -f "$RED" ] || fail "missing red harness"

grep -q 'ValueHeapRef\|encode_heap\|k_tag_heap' "$VALUE" \
  "$ROOT_DIR/runtime/include/mlc/script_vm/value_rep_abi.hpp" || \
  fail "HeapRef / heap tag missing from ValueRep"

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
printf '%s\n' "$output" | grep -q '\[script_vm\] heap_gc_arena_unit ok' || \
  fail "missing heap_gc_arena_unit ok"
printf '%s\n' "$output" | grep -q 'write_barrier_hits=' || \
  fail "missing write_barrier_hits counter"

if bash "$RED" >/tmp/script_vm_heap_gc_arena_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -Eq 'already present|already declares' /tmp/script_vm_heap_gc_arena_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/script_vm_heap_gc_arena_red.out))"

{
  echo "heap_gc_arena=ok"
  echo "red_already_present=ok"
  printf '%s\n' "$output" | grep -E 'heap_live=|write_barrier_hits=' | tail -2
} | tee "$REPORT_FILE"

echo "[script_vm heap_gc_arena_unit] ok" >&2
