#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103d — Red: no arithmetic interpreter / unit runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_interpreter_arithmetic_unit.sh
# + script_vm/interpreter.mlc + script_vm/tests/interpreter_arithmetic_unit.mlc.
# script_vm/{value,bytecode,verifier}.mlc (§103a/b/c) may already exist — unrelated.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INTERPRETER="$ROOT_DIR/script_vm/interpreter.mlc"
UNIT="$ROOT_DIR/script_vm/tests/interpreter_arithmetic_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_interpreter_arithmetic_unit.sh"

fail() {
  echo "[script_vm interpreter_arithmetic_unit_red] FAIL: $1" >&2
  exit 1
}

# Red: green artifacts must be absent.
if [ -f "$GREEN" ]; then
  fail "green interpreter arithmetic unit runner already present (expected gap until Green)"
fi
if [ -f "$INTERPRETER" ]; then
  fail "script_vm/interpreter.mlc already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "interpreter_arithmetic_unit.mlc already present (expected gap until Green)"
fi

fail "no script_vm interpreter arithmetic / unit (Red expected)"
