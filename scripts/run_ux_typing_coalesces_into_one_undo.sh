#!/usr/bin/env bash
# TRACK_EDITOR_UNDO_COALESCE STEP=1 — scenario typing_coalesces_into_one_undo (L1.5).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/typing_coalesces_into_one_undo.mlc"
OUT_DIR="${UX_TYPING_COALESCE_OUT:-$ROOT_DIR/tmp/ux_typing_coalesces_into_one_undo}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux typing_coalesce] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux typing_coalesce] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok typing_coalesces_into_one_undo'; then
  echo "[ux typing_coalesce] FAIL: missing ux_ok typing_coalesces_into_one_undo" >&2
  exit 1
fi
echo "[ux typing_coalesce] ok" >&2
