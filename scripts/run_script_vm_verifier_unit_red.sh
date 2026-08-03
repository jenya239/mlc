#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103c — Red: no bytecode verifier / unit runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_verifier_unit.sh
# + script_vm/verifier.mlc + script_vm/tests/verifier_unit.mlc.
# script_vm/{value,bytecode}.mlc (§103a/b) may already exist — unrelated.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
VERIFIER="$ROOT_DIR/script_vm/verifier.mlc"
UNIT="$ROOT_DIR/script_vm/tests/verifier_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_verifier_unit.sh"

fail() {
  echo "[script_vm verifier_unit_red] FAIL: $1" >&2
  exit 1
}

# Red: green artifacts must be absent.
if [ -f "$GREEN" ]; then
  fail "green verifier unit runner already present (expected gap until Green)"
fi
if [ -f "$VERIFIER" ]; then
  fail "script_vm/verifier.mlc already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "verifier_unit.mlc already present (expected gap until Green)"
fi

fail "no script_vm verifier / unit (Red expected)"
