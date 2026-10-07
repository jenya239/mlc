#!/usr/bin/env bash
# Probe: sqlite3.h is visible, and extern lib "sqlite3" reaches mlc_link_libs.txt.
# Exit 2 means the header is missing. That is not a green run.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"
CXX="${CXX:-clang++}"
STAGE="${1:-link}"

if [[ ! -x "$MLCC" ]]; then
  echo "[sqlite link] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <sqlite3.h>' | "$CXX" -x c++ -fsyntax-only - >/dev/null 2>&1; then
  echo "install libsqlite3-dev" >&2
  exit 2
fi

compile_probe() {
  local name="$1"
  local entry="$2"
  local out="$ROOT/tmp/sqlite_link/$name"
  mkdir -p "$ROOT/tmp/${name}_build"
  rm -rf "$out"
  mkdir -p "$out"
  TMPDIR="$ROOT/tmp/${name}_build" MLCC_OBJ_CLEAN=1 MLCC_PCH=0 \
    "$MLCC" -o "$out" "$entry"
  if ! grep -qx 'sqlite3' "$out/mlc_link_libs.txt"; then
    echo "[sqlite link] FAIL: sqlite3 missing from $out/mlc_link_libs.txt" >&2
    if [[ -f "$out/mlc_link_libs.txt" ]]; then
      cat "$out/mlc_link_libs.txt" >&2
    fi
    exit 1
  fi
  if grep -R -n 'sqlite3.h' "$out" --include='*.hpp' >/dev/null 2>&1; then
    echo "[sqlite link] FAIL: sqlite3.h leaked into a generated header" >&2
    exit 1
  fi
  PROBE_OUT="$out"
}

case "$STAGE" in
  link)
    compile_probe sqlite_link_probe "$ROOT/misc/probe/sqlite_link_probe.mlc"
    OUT="$PROBE_OUT"
    if ! grep -R -n 'sqlite3.h' "$OUT" --include='*.cpp' >/dev/null 2>&1; then
      echo "[sqlite link] FAIL: sqlite3.h missing from generated cpp" >&2
      exit 1
    fi
    MLCC_PCH=0 MLCC_OBJ_CLEAN=1 "$ROOT/compiler/build_bin.sh" "$OUT" "$OUT/probe"
    "$OUT/probe"
    ;;
  stdlib-link)
    compile_probe sqlite_stdlib_probe "$ROOT/misc/probe/sqlite_stdlib_probe.mlc"
    OUT="$PROBE_OUT"
    if ! grep -R -n 'mlc/db/sqlite_abi.hpp' "$OUT" --include='*.cpp' >/dev/null 2>&1; then
      echo "[sqlite link] FAIL: sqlite_abi.hpp missing from generated cpp" >&2
      exit 1
    fi
    MLCC_PCH=0 MLCC_OBJ_CLEAN=1 "$ROOT/compiler/build_bin.sh" "$OUT" "$OUT/probe"
    "$OUT/probe"
    ;;
  *)
    echo "[sqlite link] FAIL: unknown stage $STAGE" >&2
    exit 1
    ;;
esac
echo "[sqlite link] OK stage=$STAGE"
