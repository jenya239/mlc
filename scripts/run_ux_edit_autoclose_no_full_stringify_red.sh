#!/usr/bin/env bash
# TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY STEP=1 — autoclose still full-flattens.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
EDIT="$ROOT_DIR/misc/editor/document/edit.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_edit_autoclose_no_full_stringify_stable.sh"

if [ ! -f "$EDIT" ]; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: missing $EDIT" >&2
  exit 1
fi

if ! grep -q 'export fn edit_insert_text_autoclose(' "$EDIT"; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: missing edit_insert_text_autoclose" >&2
  exit 1
fi

# Gap: autoclose body still full-flattens (≥2 document_to_string).
autoclose_block="$(
  awk '/export fn edit_insert_text_autoclose\(/,/^export fn /' "$EDIT" | head -n -1
)"
# Fallback if next export is missing: take until blank line after end
if [ -z "$autoclose_block" ] || ! printf '%s\n' "$autoclose_block" | grep -q 'document_to_string'; then
  autoclose_block="$(
    awk '/export fn edit_insert_text_autoclose\(/,/^end$/' "$EDIT"
  )"
fi

flatten_count="$(printf '%s\n' "$autoclose_block" | grep -c 'document_to_string(document)' || true)"
if [ "$flatten_count" -lt 2 ]; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: expected ≥2 document_to_string(document) in autoclose, got $flatten_count" >&2
  exit 1
fi

# Must not already use document_byte_slice in autoclose.
if printf '%s\n' "$autoclose_block" | grep -q 'document_byte_slice'; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: document_byte_slice already in autoclose (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok edit_autoclose_no_full_stringify$' "$STABLE"; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux edit_autoclose_no_full_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok edit_autoclose_no_full_stringify_red"
echo "[ux edit_autoclose_no_full_stringify_red] ok flattens=$flatten_count" >&2
