#!/usr/bin/env bash
# Compile+link mlc::crypto sodium smoke. SKIP (exit 0) if libsodium missing.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INCLUDE="$ROOT/runtime/include"
SOURCE="$ROOT/runtime/test/test_crypto.cpp"
OUT="$ROOT/runtime/test/test_crypto"
CXX="${CXX:-g++}"

find_sodium() {
  if echo '#include <sodium.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1; then
    echo system
    return 0
  fi
  local local_prefix="$ROOT/.tmp_libsodium"
  if [[ -f "$local_prefix/usr/include/sodium.h" && -f "$local_prefix/usr/lib/x86_64-linux-gnu/libsodium.so" ]]; then
    echo local
    return 0
  fi
  return 1
}

MODE="$(find_sodium || true)"
if [[ -z "${MODE}" ]]; then
  echo "SKIP: libsodium not installed (apt install libsodium-dev, or extract to .tmp_libsodium)"
  exit 0
fi

FLAGS=(-std=c++20 -pthread "-I${INCLUDE}")
LIBS=(-lsodium)
case "$MODE" in
  local)
    FLAGS+=("-I${ROOT}/.tmp_libsodium/usr/include")
    LIBS=("-L${ROOT}/.tmp_libsodium/usr/lib/x86_64-linux-gnu" -lsodium "-Wl,-rpath,${ROOT}/.tmp_libsodium/usr/lib/x86_64-linux-gnu")
    ;;
esac

"$CXX" "${FLAGS[@]}" -o "$OUT" "$SOURCE" "${LIBS[@]}"
"$OUT"
echo "crypto runtime smoke OK (mode=$MODE)"
