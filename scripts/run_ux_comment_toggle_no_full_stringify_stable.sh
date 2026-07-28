#!/usr/bin/env bash
# TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY STEP=2 — comment toggle via slice + local replace.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"

if [ ! -f "$EDIT" ]; then
  echo "[ux comment_toggle_no_full_stringify_stable] FAIL: missing $EDIT" >&2
  exit 1
fi

toggle_block="$(
  awk '/export fn edit_toggle_line_comment\(/,/^export fn edit_backspace\(/' "$EDIT" | head -n -1
)"
if [ -z "$toggle_block" ]; then
  echo "[ux comment_toggle_no_full_stringify_stable] FAIL: missing edit_toggle_line_comment" >&2
  exit 1
fi

if printf '%s\n' "$toggle_block" | grep -q 'document_to_string\|document_from_string'; then
  echo "[ux comment_toggle_no_full_stringify_stable] FAIL: edit_toggle_line_comment still full-flattens" >&2
  exit 1
fi
if ! printf '%s\n' "$toggle_block" | grep -q 'document_byte_slice'; then
  echo "[ux comment_toggle_no_full_stringify_stable] FAIL: missing document_byte_slice" >&2
  exit 1
fi
if grep -q 'document_to_string\|document_from_string\|LineIndex' "$EDIT"; then
  echo "[ux comment_toggle_no_full_stringify_stable] FAIL: edit.mlc still imports full-stringify helpers" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_edit_unit.sh"
bash "$ROOT_DIR/scripts/run_ux_toggle_line_comment.sh"

echo "ux_ok comment_toggle_no_full_stringify"
echo "[ux comment_toggle_no_full_stringify_stable] ok" >&2
