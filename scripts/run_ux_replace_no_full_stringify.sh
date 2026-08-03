#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107l — replace via document_delete/insert (no document_from_string).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
FIND="$ROOT_DIR/misc/editor/ux/find.mlc"

fail() {
  echo "[ux replace_no_full_stringify] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$FIND" ] || fail "missing $FIND"

if grep -q 'document_from_string(replace_' "$DEMO"; then
  fail "demo_live still document_from_string(replace_*.text)"
fi
if grep -q 'fn splice_range(' "$FIND"; then
  fail "find.mlc still has splice_range"
fi
if ! grep -q 'document_delete(' "$FIND"; then
  fail "find.mlc missing document_delete"
fi
if ! grep -q 'document_insert(' "$FIND"; then
  fail "find.mlc missing document_insert"
fi

bash "$ROOT_DIR/scripts/run_ux_replace_one_and_all.sh"

echo "ux_ok replace_no_full_stringify"
echo "[ux replace_no_full_stringify] ok" >&2
