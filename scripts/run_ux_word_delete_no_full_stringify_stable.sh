#!/usr/bin/env bash
# TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY STEP=2 — word-delete via document peeks.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"
BOUNDARY="$ROOT_DIR/misc/editor/document/word_boundary.mlc"

if [ ! -f "$EDIT" ]; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: missing $EDIT" >&2
  exit 1
fi

backward_block="$(
  awk '/export fn edit_delete_word_backward\(/,/^export fn edit_delete_word_forward\(/' "$EDIT" | head -n -1
)"
forward_block="$(
  awk '/export fn edit_delete_word_forward\(/ {
    printing=1
  }
  printing {
    print
    if ($0 == "end") {
      count++
      if (count >= 1) exit
    }
  }' "$EDIT"
)"

if printf '%s\n' "$backward_block" "$forward_block" | grep -q 'document_to_string'; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: word-delete still uses document_to_string" >&2
  exit 1
fi
if ! printf '%s\n' "$backward_block" | grep -q 'word_boundary_prev_document'; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: missing word_boundary_prev_document" >&2
  exit 1
fi
if ! printf '%s\n' "$forward_block" | grep -q 'word_boundary_next_document'; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: missing word_boundary_next_document" >&2
  exit 1
fi
if ! grep -q 'export fn word_boundary_prev_document(' "$BOUNDARY"; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: missing word_boundary_prev_document export" >&2
  exit 1
fi
if ! grep -q 'export fn word_boundary_next_document(' "$BOUNDARY"; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: missing word_boundary_next_document export" >&2
  exit 1
fi
if ! grep -q 'document_byte_slice' "$BOUNDARY"; then
  echo "[ux word_delete_no_full_stringify_stable] FAIL: word_boundary missing document_byte_slice" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_word_boundary_unit.sh"
bash "$ROOT_DIR/scripts/run_editor_edit_unit.sh"
bash "$ROOT_DIR/scripts/run_ux_ctrl_backspace_deletes_word.sh"

echo "ux_ok word_delete_no_full_stringify"
echo "[ux word_delete_no_full_stringify_stable] ok" >&2
