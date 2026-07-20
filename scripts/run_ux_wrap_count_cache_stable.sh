#!/usr/bin/env bash
# TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE — scenario wrap_count_cache_stable (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/wrap_count_cache_stable.mlc"
OUT_DIR="${UX_WRAP_COUNT_CACHE_STABLE_OUT:-$ROOT_DIR/tmp/ux_wrap_count_cache_stable}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux wrap_count_cache_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux wrap_count_cache_stable] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok wrap_count_cache_stable'; then
  echo "[ux wrap_count_cache_stable] FAIL: missing ux_ok wrap_count_cache_stable" >&2
  exit 1
fi
echo "[ux wrap_count_cache_stable] ok" >&2
