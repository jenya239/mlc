#!/usr/bin/env bash
# TRACK_EDITOR_UX_L1_BACKFILL — scenario utf8_caret_display_column (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/utf8_caret_display_column.mlc"
OUT_DIR="${UX_UTF8_CARET_DISPLAY_COLUMN_OUT:-$ROOT_DIR/tmp/ux_utf8_caret_display_column}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux utf8_caret_display_column] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux utf8_caret_display_column] FAIL binary exit=$status (expected red until STEP=3)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok utf8_caret_display_column'; then
  echo "[ux utf8_caret_display_column] FAIL: missing ux_ok utf8_caret_display_column" >&2
  exit 1
fi
echo "[ux utf8_caret_display_column] ok" >&2
