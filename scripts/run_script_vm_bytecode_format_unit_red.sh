#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103b — Red: no bytecode format / unit runner.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_bytecode_format_unit.sh
# + script_vm/bytecode.mlc + script_vm/tests/bytecode_format_unit.mlc.
# script_vm/value.mlc (§103a) may already exist — unrelated to this Red gap.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BYTECODE="$ROOT_DIR/script_vm/bytecode.mlc"
UNIT="$ROOT_DIR/script_vm/tests/bytecode_format_unit.mlc"
GREEN="$ROOT_DIR/scripts/run_script_vm_bytecode_format_unit.sh"

fail() {
  echo "[script_vm bytecode_format_unit_red] FAIL: $1" >&2
  exit 1
}

# Red: green artifacts must be absent.
if [ -f "$GREEN" ]; then
  fail "green bytecode_format unit runner already present (expected gap until Green)"
fi
if [ -f "$BYTECODE" ]; then
  fail "script_vm/bytecode.mlc already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "bytecode_format_unit.mlc already present (expected gap until Green)"
fi

fail "no script_vm bytecode format / unit (Red expected)"
