#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107o — single editor_ux_text_viewport_rect (arch-lint).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UX_DIR="$ROOT_DIR/misc/editor/ux"
PROBE="$UX_DIR/probe.mlc"

fail() {
  echo "[ux viewport_rect_single] FAIL: $1" >&2
  exit 1
}

[ -f "$PROBE" ] || fail "missing $PROBE"
[ -d "$UX_DIR" ] || fail "missing $UX_DIR"

if ! grep -q 'export fn editor_ux_text_viewport_rect' "$PROBE"; then
  fail "editor_ux_text_viewport_rect missing in probe.mlc"
fi

copy_count="$(
  { rg -c --glob '*.mlc' '^fn text_viewport_rect\(' "$UX_DIR" || true; } \
    | awk -F: '{s+=$2} END {print s+0}'
)"
if [ "$copy_count" -ne 0 ]; then
  fail "private text_viewport_rect copies=$copy_count (expected 0)"
fi

call_count="$(
  { rg -c --glob '*.mlc' 'editor_ux_text_viewport_rect\(' "$UX_DIR" || true; } \
    | awk -F: '{s+=$2} END {print s+0}'
)"
if [ "$call_count" -lt 2 ]; then
  fail "editor_ux_text_viewport_rect call sites=$call_count (expected >=2)"
fi

echo "ux_ok viewport_rect_single"
echo "[ux viewport_rect_single] ok copies=0 calls=$call_count" >&2
