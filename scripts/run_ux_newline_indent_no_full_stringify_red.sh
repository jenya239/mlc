#!/usr/bin/env bash
# TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY STEP=1 — newline indent still full-flattens.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_newline_indent_no_full_stringify_stable.sh"

if [ ! -f "$EDIT" ]; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: missing $EDIT" >&2
  exit 1
fi

if ! grep -q 'export fn edit_insert_newline(' "$EDIT"; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: missing edit_insert_newline" >&2
  exit 1
fi

newline_block="$(
  awk '/export fn edit_insert_newline\(/,/^export fn /' "$EDIT" | head -n -1
)"
if [ -z "$newline_block" ] || ! printf '%s\n' "$newline_block" | grep -q 'document_to_string'; then
  newline_block="$(
    awk '/export fn edit_insert_newline\(/,/^end$/' "$EDIT"
  )"
fi

flatten_count="$(printf '%s\n' "$newline_block" | grep -c 'document_to_string' || true)"
if [ "$flatten_count" -lt 1 ]; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: expected ≥1 document_to_string in edit_insert_newline, got $flatten_count" >&2
  exit 1
fi

# Gap: must not already use document_byte_slice for indent (fix not landed).
if printf '%s\n' "$newline_block" | grep -q 'document_byte_slice'; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: document_byte_slice already in edit_insert_newline (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok newline_indent_no_full_stringify$' "$STABLE"; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux newline_indent_no_full_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok newline_indent_no_full_stringify_red"
echo "[ux newline_indent_no_full_stringify_red] ok flattens=$flatten_count" >&2
