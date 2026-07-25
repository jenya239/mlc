#!/usr/bin/env bash
# TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY STEP=1 — clipboard slice still full-flattens.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CLIPBOARD_EDIT="$ROOT_DIR/misc/editor/document/clipboard_edit.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_clipboard_slice_no_full_stringify_stable.sh"

if [ ! -f "$CLIPBOARD_EDIT" ]; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: missing $CLIPBOARD_EDIT" >&2
  exit 1
fi

if ! grep -q 'fn document_slice(' "$CLIPBOARD_EDIT"; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: missing document_slice" >&2
  exit 1
fi

# Gap: document_slice still full-flattens via document_to_string.
if ! grep -q 'document_to_string(document)' "$CLIPBOARD_EDIT"; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: expected document_to_string(document) in clipboard_edit" >&2
  exit 1
fi

# Must not already use document_byte_slice.
if grep -q 'document_byte_slice' "$CLIPBOARD_EDIT"; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: document_byte_slice already present (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok clipboard_slice_no_full_stringify' "$STABLE"; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux clipboard_slice_no_full_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok clipboard_slice_no_full_stringify_red"
echo "[ux clipboard_slice_no_full_stringify_red] ok" >&2
