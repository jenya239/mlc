#!/usr/bin/env bash
# TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY STEP=1 — comment toggle still full-flattens.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_comment_toggle_no_full_stringify_stable.sh"

if [ ! -f "$EDIT" ]; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: missing $EDIT" >&2
  exit 1
fi

if ! grep -q 'export fn edit_toggle_line_comment(' "$EDIT"; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: missing edit_toggle_line_comment" >&2
  exit 1
fi

toggle_block="$(
  awk '/export fn edit_toggle_line_comment\(/,/^export fn edit_backspace\(/' "$EDIT" | head -n -1
)"
if [ -z "$toggle_block" ]; then
  toggle_block="$(
    awk '/export fn edit_toggle_line_comment\(/ {
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
fi

to_string_count="$(printf '%s\n' "$toggle_block" | grep -c 'document_to_string' || true)"
from_string_count="$(printf '%s\n' "$toggle_block" | grep -c 'document_from_string' || true)"
if [ "$to_string_count" -lt 1 ] || [ "$from_string_count" -lt 1 ]; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: expected document_to_string + document_from_string (to=$to_string_count from=$from_string_count)" >&2
  exit 1
fi

# Gap: fix not landed yet (no document_byte_slice in toggle body).
if printf '%s\n' "$toggle_block" | grep -q 'document_byte_slice'; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: document_byte_slice already in edit_toggle_line_comment (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok comment_toggle_no_full_stringify$' "$STABLE"; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux comment_toggle_no_full_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok comment_toggle_no_full_stringify_red"
echo "[ux comment_toggle_no_full_stringify_red] ok to_string=$to_string_count from_string=$from_string_count" >&2
