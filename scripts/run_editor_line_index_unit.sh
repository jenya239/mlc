#!/usr/bin/env bash
# TRACK_EDITOR_MVP STEP=10 — LineIndex unit (no GLFW).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${EDITOR_LINE_INDEX_ENTRY:-$ROOT_DIR/misc/editor/tests/line_index_unit.mlc}"
OUT_DIR="${EDITOR_LINE_INDEX_OUT:-$ROOT_DIR/tmp/editor_line_index}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor line_index] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[editor line_index] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] line_index_unit ok'; then
  echo "[editor line_index] FAIL: missing line_index_unit ok" >&2
  exit 1
fi
echo "[editor line_index] ok" >&2
