#!/usr/bin/env bash
# TRACK_MLC_SCRIPT_VM §103i — Red: no embedding ABI / green runner / host.
# Not part of run_ux_gate. Green adds scripts/run_script_vm_embedding_abi_unit.sh
# + script_vm/embedding.mlc + embedding_abi.h + host.c + bridge.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNIT="$ROOT_DIR/script_vm/tests/embedding_abi_unit.mlc"
HOST="$ROOT_DIR/script_vm/tests/embedding_abi_host.c"
GREEN="$ROOT_DIR/scripts/run_script_vm_embedding_abi_unit.sh"
EMBEDDING="$ROOT_DIR/script_vm/embedding.mlc"
HEADER="$ROOT_DIR/runtime/include/mlc/script_vm/embedding_abi.h"
BRIDGE_A="$ROOT_DIR/runtime/src/script_vm/embedding_abi.cpp"
BRIDGE_B="$ROOT_DIR/script_vm/embedding_abi_bridge.cpp"
TRACK="$ROOT_DIR/docs/agent/TRACK_MLC_SCRIPT_VM.md"
CLOSURES_UNIT="$ROOT_DIR/scripts/run_script_vm_closures_fibers_unit.sh"

fail() {
  echo "[script_vm embedding_abi_unit_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_MLC_SCRIPT_VM.md (unexpected drift)"
[ -f "$CLOSURES_UNIT" ] || fail "missing closures_fibers unit (unexpected drift — §103h required)"

# Red: green runner / unit / host / embedding / C ABI absent until Green.
if [ -f "$GREEN" ]; then
  fail "green embedding_abi unit runner already present (expected gap until Green)"
fi
if [ -f "$UNIT" ]; then
  fail "embedding_abi_unit.mlc already present (expected gap until Green)"
fi
if [ -f "$HOST" ]; then
  fail "embedding_abi_host.c already present (expected gap until Green)"
fi
if [ -f "$EMBEDDING" ]; then
  fail "embedding.mlc already present (expected gap until Green)"
fi
if [ -f "$HEADER" ]; then
  fail "embedding_abi.h already present (expected gap until Green)"
fi
if [ -f "$BRIDGE_A" ] || [ -f "$BRIDGE_B" ]; then
  fail "embedding ABI bridge already present (expected gap until Green)"
fi

# Red: no mlc_vm_* surface yet.
if grep -REq 'mlc_vm_create|MlcVmConfig|embedding_abi' \
  "$ROOT_DIR/runtime/include/mlc/script_vm" "$ROOT_DIR/script_vm" 2>/dev/null; then
  fail "sources already declare mlc_vm_* / MlcVmConfig / embedding_abi (expected gap until Green)"
fi

# Red: no §103i Green measured paste yet.
if grep -A40 '### §103i Green measured\|Green measured (§103i)\|### Green measured (§103i)' "$TRACK" 2>/dev/null | \
  grep -Eq 'embedding_abi=ok|host_call=ok|add1=ok'; then
  fail "TRACK already has embedding_abi post-cut measured counters (expected gap until Green)"
fi

fail "no script_vm embedding ABI unit (Red expected)"
