#!/usr/bin/env bash
# TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY STEP=2 — newline indent via slice/scan.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"

if [ ! -f "$EDIT" ]; then
  echo "[ux newline_indent_no_full_stringify_stable] FAIL: missing $EDIT" >&2
  exit 1
fi

newline_block="$(
  awk '/export fn edit_insert_newline\(/,/^export fn /' "$EDIT" | head -n -1
)"
if [ -z "$newline_block" ]; then
  newline_block="$(awk '/export fn edit_insert_newline\(/,/^end$/' "$EDIT")"
fi

# Include helper just above export (document_line_start_before).
helper_and_fn="$(
  awk '/^fn document_line_start_before\(/,/^export fn comment_prefix_for_language\(/' "$EDIT" | head -n -1
)"
if [ -z "$helper_and_fn" ]; then
  helper_and_fn="$newline_block"
fi

if printf '%s\n' "$helper_and_fn" | grep -q 'document_to_string'; then
  echo "[ux newline_indent_no_full_stringify_stable] FAIL: edit_insert_newline path still uses document_to_string" >&2
  exit 1
fi
if ! printf '%s\n' "$helper_and_fn" | grep -q 'document_byte_slice'; then
  echo "[ux newline_indent_no_full_stringify_stable] FAIL: missing document_byte_slice" >&2
  exit 1
fi
if ! printf '%s\n' "$helper_and_fn" | grep -q 'document_line_start_before'; then
  echo "[ux newline_indent_no_full_stringify_stable] FAIL: missing document_line_start_before" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_edit_unit.sh"
bash "$ROOT_DIR/scripts/run_ux_newline_caret.sh"

echo "ux_ok newline_indent_no_full_stringify"
echo "[ux newline_indent_no_full_stringify_stable] ok" >&2
