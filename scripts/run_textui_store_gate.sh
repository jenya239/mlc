#!/usr/bin/env bash
# Textui chat store gate. Exit 2 when sqlite3.h is missing.
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
  echo "[textui store] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

leaks="$(grep -l 'db/sqlite.mlc' "$ROOT"/misc/textui/*.mlc | grep -v -E 'chat_database.mlc|statement_query.mlc' || true)"
if [[ -n "$leaks" ]]; then
  echo "[textui store] FAIL: sqlite import outside chat_database.mlc and statement_query.mlc" >&2
  echo "$leaks" >&2
  exit 1
fi

if grep -q 'db/sqlite.mlc' "$ROOT/misc/textui/chat_frame.mlc"; then
  echo "[textui store] FAIL: chat_frame.mlc imports sqlite" >&2
  exit 1
fi

run_program() {
  local name="$1"
  local entry="$2"
  local out="$ROOT/tmp/textui_store_gate/$name"
  local database
  database="$(mktemp /tmp/mlc_textui_store_XXXXXX.db)"
  mkdir -p "$ROOT/tmp/${name}_build"
  rm -rf "$out"
  TMPDIR="$ROOT/tmp/${name}_build" MLCC_OBJ_CLEAN=1 MLCC_PCH=0 \
    "$MLCC" -o "$out" "$entry"
  if ! grep -qx 'sqlite3' "$out/mlc_link_libs.txt"; then
    echo "[textui store] FAIL: sqlite3 missing from link libs for $name" >&2
    rm -f "$database" "${database}.v2" "${database}-journal" "${database}-wal" "${database}-shm" "${database}.v2-journal" "${database}.v2-wal" "${database}.v2-shm"
    exit 1
  fi
  if grep -R -n 'sqlite3.h' "$out" --include='*.hpp' >/dev/null 2>&1; then
    echo "[textui store] FAIL: sqlite3.h leaked into a generated header for $name" >&2
    rm -f "$database" "${database}.v2" "${database}-journal" "${database}-wal" "${database}-shm" "${database}.v2-journal" "${database}.v2-wal" "${database}.v2-shm"
    exit 1
  fi
  MLCC_PCH=0 MLCC_OBJ_CLEAN=1 "$ROOT/compiler/build_bin.sh" "$out" "$out/bin"
  set +e
  MLC_TEXTUI_DATABASE="$database" "$out/bin"
  local status=$?
  set -e
  rm -f "$database" "${database}-journal" "${database}-wal" "${database}-shm"
  if [[ "$status" -ne 0 ]]; then
    echo "[textui store] FAIL: $name exit $status" >&2
    exit "$status"
  fi
  echo "[textui store] OK $name"
}

case "$STAGE" in
  schema) run_program store_schema "$ROOT/misc/textui/test/store_schema.mlc" ;;
  messages) run_program store_messages "$ROOT/misc/textui/test/store_messages.mlc" ;;
  ring) run_program store_ring "$ROOT/misc/textui/test/store_ring.mlc" ;;
  all)
    run_program store_schema "$ROOT/misc/textui/test/store_schema.mlc"
    run_program store_messages "$ROOT/misc/textui/test/store_messages.mlc"
    run_program store_ring "$ROOT/misc/textui/test/store_ring.mlc"
    ;;
  *)
    echo "[textui store] FAIL: unknown stage $STAGE" >&2
    exit 1
    ;;
esac
