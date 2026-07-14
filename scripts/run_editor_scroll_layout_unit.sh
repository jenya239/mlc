#!/usr/bin/env bash
# TRACK_EDITOR_MVP STEP=13 — scroll layout bound (no GLFW).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${EDITOR_SCROLL_LAYOUT_ENTRY:-$ROOT_DIR/misc/editor/tests/scroll_layout_unit.mlc}"
OUT_DIR="${EDITOR_SCROLL_LAYOUT_OUT:-$ROOT_DIR/tmp/editor_scroll_layout}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor scroll_layout] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[editor scroll_layout] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] scroll_layout_unit ok'; then
  echo "[editor scroll_layout] FAIL: missing scroll_layout_unit ok" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -qE 'file_lines=100000.*bound=24'; then
  echo "[editor scroll_layout] FAIL: expected file_lines=100000 bound=24" >&2
  exit 1
fi
echo "[editor scroll_layout] ok" >&2
