#!/usr/bin/env bash
# TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY STEP=2 — autoclose via document_byte_slice.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"

if [ ! -f "$EDIT" ]; then
  echo "[ux edit_autoclose_no_full_stringify_stable] FAIL: missing $EDIT" >&2
  exit 1
fi

autoclose_block="$(
  awk '/export fn edit_insert_text_autoclose\(/,/^export fn edit_insert_newline\(/' "$EDIT" | head -n -1
)"
if [ -z "$autoclose_block" ]; then
  autoclose_block="$(awk '/export fn edit_insert_text_autoclose\(/,/^end$/' "$EDIT")"
fi

if printf '%s\n' "$autoclose_block" | grep -q 'document_to_string'; then
  echo "[ux edit_autoclose_no_full_stringify_stable] FAIL: autoclose still uses document_to_string" >&2
  exit 1
fi
if ! printf '%s\n' "$autoclose_block" | grep -q 'document_byte_slice'; then
  echo "[ux edit_autoclose_no_full_stringify_stable] FAIL: autoclose missing document_byte_slice" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_ux_typing_paren_autocloses.sh"
bash "$ROOT_DIR/scripts/run_editor_edit_unit.sh"

echo "ux_ok edit_autoclose_no_full_stringify"
echo "[ux edit_autoclose_no_full_stringify_stable] ok" >&2
