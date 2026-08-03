#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107l — Red: replace still rebuilds document from spliced string.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_replace_no_full_stringify.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
FIND="$ROOT_DIR/misc/editor/ux/find.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_replace_no_full_stringify.sh"

fail() {
  echo "[ux replace_no_full_stringify_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$FIND" ] || fail "missing $FIND"

if ! grep -q 'export fn find_replace_one(' "$FIND"; then
  fail "missing find_replace_one"
fi
if ! grep -q 'export fn find_replace_all(' "$FIND"; then
  fail "missing find_replace_all"
fi

# Red: find still splices full string.
if ! grep -q 'fn splice_range(' "$FIND"; then
  fail "splice_range already gone (expected gap until Green)"
fi

# Red: demo still rebuilds TextDocument from replace result text.
replace_from_string_count="$(
  grep -c 'document_from_string(replace_.*\.text)' "$DEMO" || true
)"
if [ "$replace_from_string_count" -lt 1 ]; then
  # Also accept the split form used today.
  replace_from_string_count="$(
    awk '
      /CmdReplaceOne|CmdReplaceAll/ { in_block=1 }
      in_block && /document_from_string\(replace_/ { count++; print }
      in_block && /^\s*\| Cmd/ && !/CmdReplace/ { in_block=0 }
    ' "$DEMO" | wc -l | tr -d " "
  )"
fi
if [ "$replace_from_string_count" -lt 1 ]; then
  fail "demo_live no longer document_from_string(replace_*.text) (expected gap until Green)"
fi

# Red: document-range apply not landed in find/demo for replace.
if grep -Eq 'document_delete\(|document_insert\(' "$FIND"; then
  fail "find.mlc already uses document_delete/insert (expected gap until Green)"
fi

# Red: green gate artifact must be absent.
if [ -f "$STABLE" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "replace still splices full string and demo rebuilds via document_from_string (Red expected)"
