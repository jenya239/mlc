#!/usr/bin/env bash
# TRACK_FFI_SHIM_MIGRATION STEP=4 — mlcc Tcp MLC+abi smoke (fd-as-token).
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_tcp_mlc}/tcp_mlc_abi_smoke"

if [[ ! -x "$MLCC" ]]; then
  echo "[tcp mlc abi] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[tcp mlc abi] codegen" >&2
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/examples/tcp_mlc_abi_smoke.mlc"

echo "[tcp mlc abi] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=tcp_mlc_abi_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/tcp_mlc_abi_smoke"

set +e
"$OUT_DIR/tcp_mlc_abi_smoke"
status=$?
set -e
if [[ "$status" -ne 0 ]]; then
  echo "[tcp mlc abi] FAIL exit=$status expected=0" >&2
  exit 1
fi
echo "[tcp mlc abi] ok exit=$status" >&2
