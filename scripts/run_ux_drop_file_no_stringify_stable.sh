#!/usr/bin/env bash
# TRACK_EDITOR_DROP_FILE_NO_STRINGIFY — L2: open dropped path without stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/drop_file_no_stringify.mlc"
OUT_DIR="${UX_DROP_FILE_NO_STRINGIFY_OUT:-$ROOT_DIR/tmp/ux_drop_file_no_stringify}"
BIN_OUT="$OUT_DIR/bin"
DROP_FILE="$ROOT_DIR/misc/editor/ux/drop_file.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux drop_file_no_stringify_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -q 'document_to_string' "$DROP_FILE"; then
  echo "[ux drop_file_no_stringify_stable] FAIL: drop_file still calls document_to_string" >&2
  exit 1
fi
if ! grep -q 'tab_set_open_buffer' "$DROP_FILE"; then
  echo "[ux drop_file_no_stringify_stable] FAIL: drop_file missing tab_set_open_buffer" >&2
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
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux drop_file_no_stringify_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok drop_file_no_stringify'; then
  echo "[ux drop_file_no_stringify_stable] FAIL: missing ux_ok drop_file_no_stringify" >&2
  exit 1
fi
echo "[ux drop_file_no_stringify_stable] ok" >&2
