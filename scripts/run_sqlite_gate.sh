#!/usr/bin/env bash
# SQLite gate: link, shim, module, statement, transaction, example.
# Exit 2 if sqlite3.h is missing. That is not a green run.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"
CXX="${CXX:-clang++}"
STAGE="${1:-all}"

if ! echo '#include <sqlite3.h>' | "$CXX" -x c++ -fsyntax-only - >/dev/null 2>&1; then
  echo "install libsqlite3-dev" >&2
  exit 2
fi

if [[ ! -x "$MLCC" ]]; then
  echo "[sqlite gate] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

run_program() {
  local name="$1"
  local entry="$2"
  local out="$ROOT/tmp/sqlite_gate/$name"
  mkdir -p "$ROOT/tmp/${name}_build"
  rm -rf "$out"
  TMPDIR="$ROOT/tmp/${name}_build" MLCC_OBJ_CLEAN=1 MLCC_PCH=0 \
    "$MLCC" -o "$out" "$entry"
  if ! grep -qx 'sqlite3' "$out/mlc_link_libs.txt"; then
    echo "[sqlite gate] FAIL: sqlite3 missing from link libs for $name" >&2
    exit 1
  fi
  if grep -R -n 'sqlite3.h' "$out" --include='*.hpp' >/dev/null 2>&1; then
    echo "[sqlite gate] FAIL: sqlite3.h leaked into a generated header for $name" >&2
    exit 1
  fi
  MLCC_PCH=0 MLCC_OBJ_CLEAN=1 "$ROOT/compiler/build_bin.sh" "$out" "$out/bin"
  if [[ "$name" == "sqlite_transaction_probe" ]]; then
    local database
    database="$(mktemp /tmp/mlc_sqlite_gate_XXXXXX.db)"
    set +e
    MLC_SQLITE_DB="$database" "$out/bin"
    local status=$?
    set -e
    rm -f "$database" "${database}-journal" "${database}-wal" "${database}-shm"
    if [[ "$status" -ne 0 ]]; then
      exit "$status"
    fi
    return 0
  fi
  "$out/bin"
}

run_link() {
  bash "$ROOT/scripts/probe_sqlite_link.sh" link
  bash "$ROOT/scripts/probe_sqlite_link.sh" stdlib-link
}

run_shim() {
  bash "$ROOT/scripts/run_sqlite_runtime_smoke.sh"
}

run_module() {
  run_program sqlite_module_probe "$ROOT/misc/probe/sqlite_module_probe.mlc"
}

run_stmt() {
  run_program sqlite_statement_probe "$ROOT/misc/probe/sqlite_statement_probe.mlc"
}

run_tx() {
  run_program sqlite_transaction_probe "$ROOT/misc/probe/sqlite_transaction_probe.mlc"
}

run_demo() {
  run_program sqlite_demo "$ROOT/misc/examples/sqlite_demo.mlc"
}

case "$STAGE" in
  link) run_link ;;
  shim) run_shim ;;
  module) run_module ;;
  stmt) run_stmt ;;
  tx) run_tx ;;
  all)
    run_link
    run_shim
    run_module
    run_stmt
    run_tx
    run_demo
    ;;
  *)
    echo "[sqlite gate] FAIL: unknown stage $STAGE" >&2
    exit 1
    ;;
esac

echo "[sqlite gate] OK stage=$STAGE"
