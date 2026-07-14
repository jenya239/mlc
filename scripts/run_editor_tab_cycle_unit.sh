#!/usr/bin/env bash
# TRACK_EDITOR_MVP STEP=22 — Ctrl+Tab next/prev unit (no GLFW).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${EDITOR_TAB_CYCLE_ENTRY:-$ROOT_DIR/misc/editor/tests/tab_cycle_unit.mlc}"
OUT_DIR="${EDITOR_TAB_CYCLE_OUT:-$ROOT_DIR/tmp/editor_tab_cycle}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor tab_cycle] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[editor tab_cycle] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] tab_cycle_unit ok'; then
  echo "[editor tab_cycle] FAIL: missing tab_cycle_unit ok" >&2
  exit 1
fi
echo "[editor tab_cycle] ok" >&2
