#!/usr/bin/env bash
# Fast dev-loop bootstrap: --emit-layout=hybrid + --cpp-mode=fast-build (few
# compile units, if-chain match dispatch instead of std::visit(overloaded{}))
# translated by an existing mlcc, built with -O0 for the fastest possible
# mlcc-stage1 compile, then times how fast that (cheap-to-build,
# slower-running) binary redoes its own default-mode translation of the
# entry. Review Step 22 (mlc-support/responses/review_20260629_144027.md:
# 453-473). Not a correctness check — see bootstrap-full.sh for that.
# Usage: compiler/scripts/bootstrap-fast.sh [entry.mlc]
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${1:-$COMPILER_DIR/main.mlc}"

if [ ! -x "$MLCC" ]; then
  echo "bootstrap-fast: missing $MLCC — run compiler/build.sh first" >&2
  exit 1
fi

source "$COMPILER_DIR/scripts/select_cxx.sh"

RT_INC="$ROOT/runtime/include"
RT_SRC=(
  "$ROOT/runtime/src/io/io.cpp"
  "$ROOT/runtime/src/core/string.cpp"
  "$ROOT/runtime/src/core/profile.cpp"
)

WORK="${TMPDIR:-/tmp}/mlc_bootstrap_fast"
rm -rf "$WORK"
mkdir -p "$WORK/stage1" "$WORK/stage2"

echo "bootstrap-fast: entry=$ENTRY mlcc=$MLCC"
"$MLCC" --emit-layout=hybrid --cpp-mode=fast-build -o "$WORK/stage1" "$ENTRY"

"${CXX_CMD[@]}" -std=c++20 -O0 "$WORK"/stage1/*.cpp "${RT_SRC[@]}" -I "$WORK/stage1" -I "$RT_INC" -o "$WORK/mlcc-stage1"

echo "bootstrap-fast: mlcc-stage1 timing its own (default-mode) re-translation of $ENTRY"
time "$WORK/mlcc-stage1" --emit-layout=hybrid -o "$WORK/stage2" "$ENTRY"

echo "bootstrap-fast: OK ($WORK/mlcc-stage1)"
