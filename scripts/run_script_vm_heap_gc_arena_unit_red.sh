#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103f — Red: no heap GC arena unit / green runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_heap_gc_arena_unit.sh
# + script_vm/tests/heap_gc_arena_unit.mlc (+ script_vm/heap.mlc / HeapRef).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNIT="$ROOT_DIR/script_vm/tests/heap_gc_arena_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_heap_gc_arena_unit.sh"
HEAP="$ROOT_DIR/script_vm/heap.mlc"
VALUE="$ROOT_DIR/script_vm/value.mlc"
VALUE_ABI="$ROOT_DIR/runtime/include/mlc/script_vm/value_rep_abi.hpp"
TRACK="$ROOT_DIR/docs/agent/TRACK_MLC_SCRIPT_VM.md"

fail() {
  echo "[script_vm heap_gc_arena_unit_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_MLC_SCRIPT_VM.md (unexpected drift)"
[ -f "$VALUE" ] || fail "missing value.mlc (unexpected drift — §103a required)"
[ -f "$VALUE_ABI" ] || fail "missing value_rep_abi.hpp (unexpected drift — §103a required)"

# Red: green runner / unit absent until Green.
if [ -f "$GREEN" ]; then
  fail "green heap_gc_arena unit runner already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "heap_gc_arena_unit.mlc already present (expected gap until Green)"
fi

# Red: heap module / HeapRef tag absent until Green.
if [ -f "$HEAP" ]; then
  fail "script_vm/heap.mlc already present (expected gap until Green)"
fi
if grep -Eq 'HeapRef|ValueHeapRef|k_tag_heap|encode_heap' "$VALUE" "$VALUE_ABI" 2>/dev/null; then
  fail "ValueRep already declares HeapRef / heap tag (expected gap until Green)"
fi

# Red: no §103f Green measured paste yet.
if grep -A40 '### §103f Green measured\|Green measured (§103f)\|### Green measured (§103f)' "$TRACK" 2>/dev/null | \
  grep -Eq 'heap_live=|write_barrier_hits=|heap_gc_arena=ok'; then
  fail "TRACK already has heap_gc_arena post-cut measured counters (expected gap until Green)"
fi

fail "no script_vm heap_gc_arena unit (Red expected)"
