#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103d — arithmetic interpreter unit.
# Not part of run_ux_gate / dev_gate_fast this sub-track.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/script_vm/tests/interpreter_arithmetic_unit.mlc"
OUT_DIR="${SCRIPT_VM_INTERPRETER_OUT:-$ROOT_DIR/tmp/script_vm_interpreter}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[script_vm interpreter_arithmetic_unit] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ROOT_DIR/script_vm/interpreter.mlc" ]; then
  echo "[script_vm interpreter_arithmetic_unit] FAIL: missing script_vm/interpreter.mlc" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[script_vm interpreter_arithmetic_unit] FAIL: missing $ENTRY" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"

set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[script_vm interpreter_arithmetic_unit] FAIL exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[script_vm\] interpreter_arithmetic_unit ok'; then
  echo "[script_vm interpreter_arithmetic_unit] FAIL: missing interpreter_arithmetic_unit ok" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[script_vm\] interpreter_arithmetic perf'; then
  echo "[script_vm interpreter_arithmetic_unit] FAIL: missing perf line" >&2
  exit 1
fi

echo "[script_vm interpreter_arithmetic_unit] ok" >&2
