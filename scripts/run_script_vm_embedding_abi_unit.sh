#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103i — embedding ABI unit + host C link.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/script_vm/tests/embedding_abi_unit.mlc"
EMBEDDING="$ROOT_DIR/script_vm/embedding.mlc"
HOST="$ROOT_DIR/script_vm/tests/embedding_abi_host.cpp"
BRIDGE="$ROOT_DIR/runtime/src/script_vm/embedding_abi.cpp"
HEADER="$ROOT_DIR/runtime/include/mlc/script_vm/embedding_abi.h"
RED="$ROOT_DIR/scripts/run_script_vm_embedding_abi_unit_red.sh"
CLOSURES_UNIT="$ROOT_DIR/scripts/run_script_vm_closures_fibers_unit.sh"
OUT_DIR="${SCRIPT_VM_EMBEDDING_ABI_OUT:-$ROOT_DIR/tmp/script_vm_embedding_abi}"
UNIT_OUT="$OUT_DIR/unit"
HOST_OUT="$OUT_DIR/host"
UNIT_BIN="$OUT_DIR/unit_bin"
HOST_BIN="$OUT_DIR/host_bin"
REPORT_DIR="${SCRIPT_VM_EMBEDDING_ABI_REPORT:-$ROOT_DIR/.tmp/script_vm_embedding_abi}"
REPORT_FILE="$REPORT_DIR/report.txt"

fail() {
  echo "[script_vm embedding_abi_unit] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$EMBEDDING" ] || fail "missing embedding.mlc"
[ -f "$HOST" ] || fail "missing host"
[ -f "$BRIDGE" ] || fail "missing bridge"
[ -f "$HEADER" ] || fail "missing embedding_abi.h"
[ -f "$RED" ] || fail "missing red harness"

grep -Eq 'emb_call_export_bytes|emb_encode_add1_module|mlc_vm_create' \
  "$EMBEDDING" "$HEADER" "$BRIDGE" || fail "embedding ABI surface missing"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR" "$REPORT_DIR"
mkdir -p "$OUT_DIR" "$REPORT_DIR"

"$MLCC" -o "$UNIT_OUT" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$UNIT_OUT" "$UNIT_BIN"
[ -x "$UNIT_BIN" ] || fail "missing unit binary"

set +e
unit_output=$("$UNIT_BIN" 2>&1)
unit_status=$?
set -e
printf '%s\n' "$unit_output"
if [ "$unit_status" -ne 0 ]; then
  fail "unit exit=$unit_status"
fi
printf '%s\n' "$unit_output" | grep -q '\[script_vm\] embedding_abi_unit ok' || \
  fail "missing embedding_abi_unit ok"
printf '%s\n' "$unit_output" | grep -q 'embedding_abi=ok' || fail "missing embedding_abi=ok"
printf '%s\n' "$unit_output" | grep -q 'add1=ok' || fail "missing unit add1=ok"

# Host: mlcc embedding.mlc (no main) + bridge + host.cpp
"$MLCC" -o "$HOST_OUT" "$EMBEDDING"
cp "$BRIDGE" "$HOST_OUT/embedding_abi.cpp"
cp "$HOST" "$HOST_OUT/embedding_abi_host.cpp"
"$COMPILER_DIR/build_bin.sh" "$HOST_OUT" "$HOST_BIN"
[ -x "$HOST_BIN" ] || fail "missing host binary"

set +e
host_output=$("$HOST_BIN" 2>&1)
host_status=$?
set -e
printf '%s\n' "$host_output"
if [ "$host_status" -ne 0 ]; then
  fail "host exit=$host_status"
fi
printf '%s\n' "$host_output" | grep -q '\[script_vm\] embedding_abi_host ok' || \
  fail "missing embedding_abi_host ok"
printf '%s\n' "$host_output" | grep -q 'host_call=ok' || fail "missing host_call=ok"

if bash "$RED" >/tmp/script_vm_embedding_abi_red.out 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -Eq 'already present|already declare' /tmp/script_vm_embedding_abi_red.out || \
  fail "red did not report already-present ($(head -1 /tmp/script_vm_embedding_abi_red.out))"

bash "$CLOSURES_UNIT" >/tmp/script_vm_embedding_abi_closures_side.out 2>&1 || \
  fail "side closures_fibers unit failed"

{
  echo "embedding_abi=ok"
  echo "add1=ok"
  echo "host_call=ok"
  echo "red_already_present=ok"
  echo "side_closures_fibers=ok"
} | tee "$REPORT_FILE"

echo "[script_vm embedding_abi_unit] ok" >&2
