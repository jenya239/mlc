#!/usr/bin/env bash
# TRACK_EDITOR_ENCODING_GUARD STEP=1 — L0 utf8_bom_detected.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/tests/utf8_bom_detected.mlc"
OUT_DIR="${EDITOR_UTF8_BOM_OUT:-$ROOT_DIR/tmp/editor_utf8_bom_detected}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor bom] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
mkdir -p "$ROOT_DIR/.tmp/editor_fixture"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$(cd "$ROOT_DIR" && "$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[editor bom] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] utf8_bom_detected ok'; then
  echo "[editor bom] FAIL: missing utf8_bom_detected ok" >&2
  exit 1
fi
echo "[editor bom] ok" >&2
