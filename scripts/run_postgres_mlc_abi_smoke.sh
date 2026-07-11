#!/usr/bin/env bash
# TRACK_FFI_SHIM_MIGRATION STEP=2 — mlcc Postgres MLC+abi smoke (no live DB).
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_postgres_mlc}/postgres_mlc_abi_smoke"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[postgres mlc abi] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

find_libpq() {
  if echo '#include <libpq-fe.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1; then
    echo system
    return 0
  fi
  if echo '#include <postgresql/libpq-fe.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1; then
    echo system_pg
    return 0
  fi
  local local_prefix="$ROOT_DIR/.tmp_libpq"
  if [[ -f "$local_prefix/usr/include/postgresql/libpq-fe.h" && -f "$local_prefix/usr/lib/x86_64-linux-gnu/libpq.so" ]]; then
    echo local
    return 0
  fi
  return 1
}

MODE="$(find_libpq || true)"
if [[ -z "$MODE" ]]; then
  echo "[postgres mlc abi] SKIP: libpq headers missing" >&2
  exit 0
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[postgres mlc abi] codegen" >&2
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/examples/postgres_mlc_abi_smoke.mlc"
printf 'pq\n' > "$OUT_DIR/mlc_link_libs.txt"

echo "[postgres mlc abi] link" >&2
EXTRA_INC=""
if [[ "$MODE" == "local" ]]; then
  EXTRA_INC="${ROOT_DIR}/.tmp_libpq/usr/include/postgresql"
  export LIBRARY_PATH="${ROOT_DIR}/.tmp_libpq/usr/lib/x86_64-linux-gnu${LIBRARY_PATH:+:$LIBRARY_PATH}"
  export LD_LIBRARY_PATH="${ROOT_DIR}/.tmp_libpq/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=postgres_mlc_abi_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/postgres_mlc_abi_smoke" "$EXTRA_INC"

set +e
"$OUT_DIR/postgres_mlc_abi_smoke"
status=$?
set -e
if [[ "$status" -ne 0 ]]; then
  echo "[postgres mlc abi] FAIL exit=$status expected=0" >&2
  exit 1
fi
echo "[postgres mlc abi] ok exit=$status" >&2
