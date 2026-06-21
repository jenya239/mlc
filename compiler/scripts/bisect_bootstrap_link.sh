#!/usr/bin/env bash
# git bisect helper for MLCC_BOOTSTRAP link.
# exit 0 — Ruby-built mlcc runs, self-emits compiler C++, g++ produces mlcc_bootstrap
# exit 1 — self-compile or link failed (regression)
# exit 125 — Ruby modular compiler failed to build mlcc (skip commit)
set -e
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
MAIN_CLONE_VENDOR="$ROOT/../mlc/vendor/bundle"
if [[ -d "$MAIN_CLONE_VENDOR" ]]; then
  export BUNDLE_PATH="$MAIN_CLONE_VENDOR"
fi
LOG="${TMPDIR:-/tmp}/mlc_bisect_bootstrap.log"
: >"$LOG"
if ! MLCC_FORCE_RUBY=1 compiler/build.sh >>"$LOG" 2>&1; then
  exit 125
fi
BIN="$ROOT/compiler/out/mlcc"
BS="$ROOT/compiler/out/bootstrap_probe"
rm -rf "$BS" "$ROOT/compiler/out/bootstrap/obj" "$ROOT/compiler/out/bootstrap/mlcc_bootstrap" 2>/dev/null || true
mkdir -p "$BS"
if ! (cd "$ROOT/compiler" && "$BIN" main.mlc -o "$BS" >>"$LOG" 2>&1); then
  exit 125
fi
RT_INC="$ROOT/runtime/include"
RT_CPP="$ROOT/runtime/src/core/string.cpp $ROOT/runtime/src/core/profile.cpp $ROOT/runtime/src/io/io.cpp"
OBJ_DIR="$BS/obj"
mkdir -p "$OBJ_DIR"
if (
  for cpp in "$BS"/*.cpp; do
    [[ -f "$cpp" ]] || continue
    g++ -std=c++20 -O2 -I "$BS" -I "$RT_INC" -c "$cpp" -o "$OBJ_DIR/$(basename "$cpp" .cpp).o"
  done
  for rcpp in $RT_CPP; do
    [[ -f "$rcpp" ]] && g++ -std=c++20 -O2 -I "$RT_INC" -c "$rcpp" -o "$OBJ_DIR/runtime_$(basename "$rcpp" .cpp).o"
  done
  g++ -std=c++20 -O2 -o "$BS/mlcc_bootstrap" "$OBJ_DIR"/*.o
); then
  [[ -x "$BS/mlcc_bootstrap" ]] || exit 1
  exit 0
fi
exit 1
