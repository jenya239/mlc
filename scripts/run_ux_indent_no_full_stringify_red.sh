#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107l — Red: indent/outdent still full-stringify.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_indent_no_full_stringify.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INDENT="$ROOT_DIR/misc/editor/document/indent.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_indent_no_full_stringify.sh"

fail() {
  echo "[ux indent_no_full_stringify_red] FAIL: $1" >&2
  exit 1
}

[ -f "$INDENT" ] || fail "missing $INDENT"

if ! grep -q 'export fn edit_indent_tab(' "$INDENT"; then
  fail "missing edit_indent_tab"
fi
if ! grep -q 'export fn edit_outdent_lines(' "$INDENT"; then
  fail "missing edit_outdent_lines"
fi

# Red: flatten/rebuild still present.
if ! grep -q 'document_to_string(document)' "$INDENT"; then
  fail "indent no longer calls document_to_string (expected gap until Green)"
fi
if ! grep -q 'document_from_string(next_text)' "$INDENT"; then
  fail "indent no longer calls document_from_string (expected gap until Green)"
fi
if ! grep -q 'fn rebuild_lines_with_transform(' "$INDENT"; then
  fail "rebuild_lines_with_transform already gone (expected gap until Green)"
fi

# Red: range-edit fix not landed yet.
if grep -q 'document_byte_slice(' "$INDENT"; then
  fail "document_byte_slice already in indent.mlc (expected gap until Green)"
fi
if grep -q 'document_delete(' "$INDENT"; then
  fail "document_delete already in indent.mlc (expected gap until Green)"
fi

# Red: green gate artifact must be absent.
if [ -f "$STABLE" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "indent/outdent still document_to_string + rebuild + document_from_string (Red expected)"
