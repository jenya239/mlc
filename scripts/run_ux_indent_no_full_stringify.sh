#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107l — indent/outdent without full stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INDENT="$ROOT_DIR/misc/editor/document/indent.mlc"

fail() {
  echo "[ux indent_no_full_stringify] FAIL: $1" >&2
  exit 1
}

[ -f "$INDENT" ] || fail "missing $INDENT"

if grep -q 'document_to_string\|document_from_string\|rebuild_lines_with_transform' "$INDENT"; then
  fail "indent.mlc still full-flattens / rebuilds lines"
fi
if ! grep -q 'document_byte_slice(' "$INDENT"; then
  fail "missing document_byte_slice"
fi
if ! grep -q 'document_delete(' "$INDENT"; then
  fail "missing document_delete"
fi
if ! grep -q 'document_insert(' "$INDENT"; then
  fail "missing document_insert"
fi

bash "$ROOT_DIR/scripts/run_ux_tab_shift_tab_indents.sh"

echo "ux_ok indent_no_full_stringify"
echo "[ux indent_no_full_stringify] ok" >&2
