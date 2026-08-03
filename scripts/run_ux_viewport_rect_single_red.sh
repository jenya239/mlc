#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107o — Red: eight private text_viewport_rect copies.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_viewport_rect_single.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UX_DIR="$ROOT_DIR/misc/editor/ux"
PROBE="$UX_DIR/probe.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_viewport_rect_single.sh"

fail() {
  echo "[ux viewport_rect_single_red] FAIL: $1" >&2
  exit 1
}

[ -d "$UX_DIR" ] || fail "missing $UX_DIR"
[ -f "$PROBE" ] || fail "missing $PROBE"

copy_count="$(
  rg -c --glob '*.mlc' '^fn text_viewport_rect\(' "$UX_DIR" 2>/dev/null \
    | awk -F: '{s+=$2} END {print s+0}'
)"
if [ "$copy_count" -lt 2 ]; then
  fail "expected multiple private text_viewport_rect copies, found $copy_count"
fi
if [ "$copy_count" -ne 8 ]; then
  fail "expected 8 private text_viewport_rect copies, found $copy_count"
fi

# Red: shared export not shipped yet.
if grep -q 'export fn editor_ux_text_viewport_rect' "$PROBE"; then
  fail "editor_ux_text_viewport_rect already present (expected gap until Green)"
fi

# Red: green gate artifact must be absent.
if [ -f "$STABLE" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "8 private text_viewport_rect copies in ux/* (Red expected)"
