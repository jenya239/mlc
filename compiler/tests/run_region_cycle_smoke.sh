#!/usr/bin/env bash
# TRACK_LANG_REGION_ARENA STEP=6 — cyclic region graph; no Shared atomic in gen.
set -euo pipefail
COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
SRC="$COMPILER_DIR/tests/e2e/region_cycle.mlc"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC="$ROOT_DIR/runtime/src/io/io.cpp $ROOT_DIR/runtime/src/core/string.cpp"
source "$ROOT_DIR/compiler/scripts/select_cxx.sh"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

"$MLCC" -o "$tmpdir" "$SRC"

if ! rg -q 'RegionHandle' "$tmpdir"/*.cpp; then
  echo "FAIL expected RegionHandle in generated C++" >&2
  exit 1
fi
if ! rg -q '\.alloc\(' "$tmpdir"/*.cpp; then
  echo "FAIL expected .alloc( in generated C++" >&2
  exit 1
fi
if rg -q 'shared_ptr|make_shared|std::atomic' "$tmpdir"/*.cpp; then
  echo "FAIL Shared/atomic found in regional allocation TU:" >&2
  rg -n 'shared_ptr|make_shared|std::atomic' "$tmpdir"/*.cpp >&2 || true
  exit 1
fi

"${CXX_CMD[@]}" -std=c++20 -I "$tmpdir" -I "$RT_INC" "$tmpdir"/*.cpp $RT_SRC -o "$tmpdir/prog"
actual="$("$tmpdir/prog" 2>&1)"
if [ "$actual" != "cycle_ok" ]; then
  echo "FAIL expected cycle_ok, got: $actual" >&2
  exit 1
fi

echo "region_cycle_smoke: ok"
