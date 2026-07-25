#!/usr/bin/env bash
# TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY STEP=1 — word-delete still full-flattens.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_word_delete_no_full_stringify_stable.sh"

if [ ! -f "$EDIT" ]; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: missing $EDIT" >&2
  exit 1
fi

if ! grep -q 'export fn edit_delete_word_backward(' "$EDIT"; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: missing edit_delete_word_backward" >&2
  exit 1
fi
if ! grep -q 'export fn edit_delete_word_forward(' "$EDIT"; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: missing edit_delete_word_forward" >&2
  exit 1
fi

backward_block="$(
  awk '/export fn edit_delete_word_backward\(/,/^export fn edit_delete_word_forward\(/' "$EDIT" | head -n -1
)"
forward_block="$(
  awk '/export fn edit_delete_word_forward\(/,/^end$/' "$EDIT" | tail -n +1
)"
# Trim forward_block to first function end: take until blank after final end of fn
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

backward_flattens="$(printf '%s\n' "$backward_block" | grep -c 'document_to_string' || true)"
forward_flattens="$(printf '%s\n' "$forward_block" | grep -c 'document_to_string' || true)"
if [ "$backward_flattens" -lt 1 ] || [ "$forward_flattens" -lt 1 ]; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: expected document_to_string in both word-delete fns (backward=$backward_flattens forward=$forward_flattens)" >&2
  exit 1
fi

if printf '%s\n' "$backward_block" "$forward_block" | grep -q 'document_byte_slice'; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: document_byte_slice already in word-delete (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok word_delete_no_full_stringify$' "$STABLE"; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux word_delete_no_full_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok word_delete_no_full_stringify_red"
echo "[ux word_delete_no_full_stringify_red] ok backward=$backward_flattens forward=$forward_flattens" >&2
