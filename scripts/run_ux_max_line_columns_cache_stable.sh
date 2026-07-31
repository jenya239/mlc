#!/usr/bin/env bash
# TRACK_EDITOR_HORIZONTAL_SCROLLBAR_MAX_COLUMNS_CACHE — scenario max_line_columns_cache_stable (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/max_line_columns_cache_stable.mlc"
OUT_DIR="${UX_MAX_LINE_COLUMNS_CACHE_STABLE_OUT:-$ROOT_DIR/tmp/ux_max_line_columns_cache_stable}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux max_line_columns_cache_stable] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux max_line_columns_cache_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok max_line_columns_cache_stable'; then
  echo "[ux max_line_columns_cache_stable] FAIL: missing ux_ok max_line_columns_cache_stable" >&2
  exit 1
fi
echo "[ux max_line_columns_cache_stable] ok" >&2
