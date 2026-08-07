#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_GLYPH_X §111a1 STEP=1 Red — clamp/ensure unwired.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
CHECK="$ROOT_DIR/scripts/check_scroll_note_neighbors.rb"
GREEN="$ROOT_DIR/scripts/run_editor_scroll_content_dirty.sh"

fail() {
  echo "[editor scroll glyph x red s1] FAIL: $*" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live"
[ -f "$CHECK" ] || fail "missing neighbor check"

set +e
ruby "$CHECK" "$DEMO"
status=$?
set -e

if [ "$status" -eq 0 ]; then
  # Wired → red must still fail with already present once green owns the check.
  if grep -q 'check_scroll_note_neighbors' "$GREEN" 2>/dev/null; then
    echo "[editor scroll glyph x red s1] FAIL: already present" >&2
    exit 1
  fi
  fail "neighbors ok but green missing check_scroll_note_neighbors"
fi

echo "[editor scroll glyph x red s1] FAIL: clamp/ensure missing note (Red expected)" >&2
exit 1
