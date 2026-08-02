#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107c — visible-row collect jumps via prefix_visual_rows.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/visible_rows_prefix_jump.mlc"
OUT_DIR="${UX_VISIBLE_ROWS_PREFIX_JUMP_OUT:-$ROOT_DIR/tmp/ux_visible_rows_prefix_jump}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux visible_rows_prefix_jump] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux visible_rows_prefix_jump] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok visible_rows_prefix_jump'; then
  echo "[ux visible_rows_prefix_jump] FAIL: missing ux_ok" >&2
  exit 1
fi
echo "[ux visible_rows_prefix_jump] ok" >&2
