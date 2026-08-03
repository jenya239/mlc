#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107m — oversized open refused (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/oversized_file_refused.mlc"
OUT_DIR="${UX_OVERSIZED_FILE_REFUSED_OUT:-$ROOT_DIR/tmp/ux_oversized_file_refused}"
BIN_OUT="$OUT_DIR/bin"
SAVE="$ROOT_DIR/misc/editor/document/save.mlc"
FILE_ABI="$ROOT_DIR/runtime/include/mlc/io/file_abi.hpp"

fail() {
  echo "[ux oversized_file_refused] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$SAVE" ] || fail "missing $SAVE"
[ -f "$FILE_ABI" ] || fail "missing $FILE_ABI"

if ! grep -q 'editor_max_open_bytes' "$SAVE"; then
  fail "editor_max_open_bytes missing"
fi
if ! grep -q 'MLC_EDITOR_MAX_OPEN_BYTES' "$SAVE"; then
  fail "MLC_EDITOR_MAX_OPEN_BYTES missing"
fi
if ! grep -q 'file_byte_size' "$SAVE"; then
  fail "file_byte_size missing in save.mlc"
fi
if ! grep -q 'file_byte_size_value' "$FILE_ABI"; then
  fail "file_byte_size_value missing in file_abi.hpp"
fi
if ! grep -q 'open_text_validate' "$SAVE"; then
  fail "open_text_validate missing (merged NUL/UTF-8 pass)"
fi
if grep -q 'text_contains_nul(raw)' "$SAVE"; then
  fail "open path still calls text_contains_nul(raw)"
fi
if grep -q 'utf8_text_is_valid(body)' "$SAVE"; then
  fail "open path still calls utf8_text_is_valid(body)"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_EDITOR_MAX_OPEN_BYTES=32

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR" "$ROOT_DIR/.tmp/ux_oversized_file_refused"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  fail "binary exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok oversized_file_refused'; then
  fail "missing ux_ok oversized_file_refused"
fi
echo "[ux oversized_file_refused] ok" >&2
