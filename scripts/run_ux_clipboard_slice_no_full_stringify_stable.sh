#!/usr/bin/env bash
# TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY — L2: piece-range clipboard slice.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CLIPBOARD_EDIT="$ROOT_DIR/misc/editor/document/clipboard_edit.mlc"
PIECE_TABLE="$ROOT_DIR/misc/editor/document/piece_table.mlc"

if [ ! -f "$CLIPBOARD_EDIT" ]; then
  echo "[ux clipboard_slice_no_full_stringify_stable] FAIL: missing $CLIPBOARD_EDIT" >&2
  exit 1
fi
if [ ! -f "$PIECE_TABLE" ]; then
  echo "[ux clipboard_slice_no_full_stringify_stable] FAIL: missing $PIECE_TABLE" >&2
  exit 1
fi

if grep -q 'document_to_string' "$CLIPBOARD_EDIT"; then
  echo "[ux clipboard_slice_no_full_stringify_stable] FAIL: clipboard_edit still uses document_to_string" >&2
  exit 1
fi
if ! grep -q 'document_byte_slice' "$CLIPBOARD_EDIT"; then
  echo "[ux clipboard_slice_no_full_stringify_stable] FAIL: clipboard_edit missing document_byte_slice" >&2
  exit 1
fi
if ! grep -q 'export fn document_byte_slice(' "$PIECE_TABLE"; then
  echo "[ux clipboard_slice_no_full_stringify_stable] FAIL: piece_table missing document_byte_slice" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_clipboard_unit.sh"
bash "$ROOT_DIR/scripts/run_ux_copy_paste.sh"

echo "ux_ok clipboard_slice_no_full_stringify"
echo "[ux clipboard_slice_no_full_stringify_stable] ok" >&2
