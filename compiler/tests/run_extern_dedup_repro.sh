#!/usr/bin/env bash
# TRACK_FFI_EXTERN_DEDUP STEP=2 — baseline: identical-sig redeclare → clang fail.
# After STEP=3 Hybrid fix, invert expect_clang_fail → expect_build_ok.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/extern_dedup/redeclare.mlc"
WORK="${TMPDIR:-/tmp}/mlc_extern_dedup_repro_$$"
OUT="$WORK/out"
BIN="$WORK/bin"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

"$MLCC" -o "$OUT" "$FIXTURE"
set +e
BUILD_LOG="$WORK/build.log"
"$COMPILER_DIR/build_bin.sh" "$OUT" "$BIN" >"$BUILD_LOG" 2>&1
BUILD_EXIT=$?
set -e

if [[ "$BUILD_EXIT" -eq 0 ]]; then
  echo "FAIL: expected clang fail on identical extern redeclare; build succeeded" >&2
  exit 1
fi

if ! grep -q "does not enclose namespace" "$BUILD_LOG"; then
  echo "FAIL: expected namespace-enclose clang diagnostic; got:" >&2
  cat "$BUILD_LOG" >&2
  exit 1
fi

echo "extern_dedup_repro: clang fail as expected (baseline pre-STEP=3)"
