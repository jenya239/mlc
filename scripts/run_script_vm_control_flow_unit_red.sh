#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103e — Red: no control-flow unit / green runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_control_flow_unit.sh
# + script_vm/tests/control_flow_unit.mlc (and extends bytecode/verifier/interpreter).
# script_vm/interpreter.mlc (§103d) may already exist — unrelated to this Red gap.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNIT="$ROOT_DIR/script_vm/tests/control_flow_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_control_flow_unit.sh"

fail() {
  echo "[script_vm control_flow_unit_red] FAIL: $1" >&2
  exit 1
}

if [ -f "$GREEN" ]; then
  fail "green control_flow unit runner already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "control_flow_unit.mlc already present (expected gap until Green)"
fi

fail "no script_vm control_flow unit (Red expected)"
