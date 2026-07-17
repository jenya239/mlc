#!/usr/bin/env bash
# TRACK_EDITOR_WORD_BOUNDARIES STEP=1 — scenario ctrl_arrow_word_jump (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/ctrl_arrow_word_jump.mlc"
OUT_DIR="${UX_CTRL_ARROW_WORD_OUT:-$ROOT_DIR/tmp/ux_ctrl_arrow_word_jump}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux ctrl_arrow_word] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux ctrl_arrow_word] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok ctrl_arrow_word_jump'; then
  echo "[ux ctrl_arrow_word] FAIL: missing ux_ok ctrl_arrow_word_jump" >&2
  exit 1
fi
echo "[ux ctrl_arrow_word] ok" >&2
