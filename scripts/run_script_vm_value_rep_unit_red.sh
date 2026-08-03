#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103a — Red: no script_vm ValueRep / unit runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_value_rep_unit.sh
# + script_vm/value.mlc + script_vm/tests/value_rep_unit.mlc.
# Note: compiler/vm/value.mlc is MIR VM (TRACK_MIR_VM_FULL) — unrelated.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
VALUE="$ROOT_DIR/script_vm/value.mlc"
UNIT="$ROOT_DIR/script_vm/tests/value_rep_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_value_rep_unit.sh"

fail() {
  echo "[script_vm value_rep_unit_red] FAIL: $1" >&2
  exit 1
}

# Red: green artifacts must be absent.
if [ -f "$GREEN" ]; then
  fail "green value_rep unit runner already present (expected gap until Green)"
fi
if [ -f "$VALUE" ]; then
  fail "script_vm/value.mlc already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "value_rep_unit.mlc already present (expected gap until Green)"
fi
if [ -d "$ROOT_DIR/script_vm" ]; then
  fail "script_vm/ directory already present (expected gap until Green)"
fi

fail "no script_vm ValueRep / value_rep unit (Red expected)"
