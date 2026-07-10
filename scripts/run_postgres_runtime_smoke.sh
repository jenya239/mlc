#!/usr/bin/env bash
# Compile+link mlc::db postgres smoke. SKIP (exit 0) if libpq headers/libs missing.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INCLUDE="$ROOT/runtime/include"
SOURCE="$ROOT/runtime/test/test_postgres.cpp"
OUT="$ROOT/runtime/test/test_postgres"
CXX="${CXX:-g++}"

find_libpq() {
  if echo '#include <libpq-fe.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1; then
    echo system
    return 0
  fi
  if echo '#include <postgresql/libpq-fe.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1; then
    echo system_postgresql
    return 0
  fi
  local local_prefix="$ROOT/.tmp_libpq"
  if [[ -f "$local_prefix/usr/include/postgresql/libpq-fe.h" && -f "$local_prefix/usr/lib/x86_64-linux-gnu/libpq.so" ]]; then
    echo local
    return 0
  fi
  return 1
}

MODE="$(find_libpq || true)"
if [[ -z "${MODE}" ]]; then
  echo "SKIP: libpq not installed (apt install libpq-dev, or extract to .tmp_libpq)"
  exit 0
fi

FLAGS=(-std=c++20 -pthread "-I${INCLUDE}")
LIBS=(-lpq)
case "$MODE" in
  system_postgresql)
    FLAGS+=(-I/usr/include/postgresql)
    ;;
  local)
    FLAGS+=("-I${ROOT}/.tmp_libpq/usr/include/postgresql")
    LIBS=("-L${ROOT}/.tmp_libpq/usr/lib/x86_64-linux-gnu" -lpq "-Wl,-rpath,${ROOT}/.tmp_libpq/usr/lib/x86_64-linux-gnu")
    ;;
esac

"$CXX" "${FLAGS[@]}" -o "$OUT" "$SOURCE" "${LIBS[@]}"
"$OUT"
echo "postgres runtime smoke OK (mode=$MODE)"
