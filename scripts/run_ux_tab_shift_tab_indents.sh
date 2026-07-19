#!/usr/bin/env bash
# TRACK_EDITOR_INDENT_MODEL STEP=2 — scenario tab_shift_tab_indents (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/tab_shift_tab_indents.mlc"
OUT_DIR="${UX_TAB_SHIFT_TAB_INDENTS_OUT:-$ROOT_DIR/tmp/ux_tab_shift_tab_indents}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux tab_shift_tab_indents] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux tab_shift_tab_indents] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok tab_shift_tab_indents'; then
  echo "[ux tab_shift_tab_indents] FAIL: missing ux_ok tab_shift_tab_indents" >&2
  exit 1
fi
echo "[ux tab_shift_tab_indents] ok" >&2
