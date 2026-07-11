#!/usr/bin/env bash
# TRACK_FFI_SHIM_MIGRATION STEP=3 — mlcc Crypto MLC+abi smoke (known vectors).
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_crypto_mlc}/crypto_mlc_abi_smoke"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[crypto mlc abi] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

find_sodium() {
  if echo '#include <sodium.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1; then
    echo system
    return 0
  fi
  local local_prefix="$ROOT_DIR/.tmp_libsodium"
  if [[ -f "$local_prefix/usr/include/sodium.h" && -f "$local_prefix/usr/lib/x86_64-linux-gnu/libsodium.so" ]]; then
    echo local
    return 0
  fi
  return 1
}

MODE="$(find_sodium || true)"
if [[ -z "$MODE" ]]; then
  echo "[crypto mlc abi] SKIP: libsodium headers missing" >&2
  exit 0
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[crypto mlc abi] codegen" >&2
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/examples/crypto_mlc_abi_smoke.mlc"
printf 'sodium\n' > "$OUT_DIR/mlc_link_libs.txt"

echo "[crypto mlc abi] link" >&2
EXTRA_INC=""
if [[ "$MODE" == "local" ]]; then
  EXTRA_INC="${ROOT_DIR}/.tmp_libsodium/usr/include"
  export LIBRARY_PATH="${ROOT_DIR}/.tmp_libsodium/usr/lib/x86_64-linux-gnu${LIBRARY_PATH:+:$LIBRARY_PATH}"
  export LD_LIBRARY_PATH="${ROOT_DIR}/.tmp_libsodium/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=crypto_mlc_abi_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/crypto_mlc_abi_smoke" "$EXTRA_INC"

set +e
"$OUT_DIR/crypto_mlc_abi_smoke"
status=$?
set -e
if [[ "$status" -ne 0 ]]; then
  echo "[crypto mlc abi] FAIL exit=$status expected=0" >&2
  exit 1
fi
echo "[crypto mlc abi] ok exit=$status" >&2
