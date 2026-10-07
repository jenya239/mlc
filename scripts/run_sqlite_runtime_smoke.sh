#!/usr/bin/env bash
# Compile and run the SQLite ABI smoke. Exit 2 if sqlite3.h is missing.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CXX="${CXX:-clang++}"
SOURCE="$ROOT/runtime/test/sqlite_abi_smoke.cpp"
OUT="$ROOT/tmp/sqlite_abi_smoke"

if ! echo '#include <sqlite3.h>' | "$CXX" -x c++ -fsyntax-only - >/dev/null 2>&1; then
  echo "install libsqlite3-dev" >&2
  exit 2
fi

mkdir -p "$ROOT/tmp"
"$CXX" -std=c++20 -pthread -I"$ROOT/runtime/include" -o "$OUT" "$SOURCE" -lsqlite3
"$OUT"
echo "[sqlite shim] OK"
