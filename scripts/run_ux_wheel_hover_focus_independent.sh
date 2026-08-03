#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107r — behavioural L1: wheel target follows hover, not focus.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/wheel_hover_focus_independent.mlc"
OUT_DIR="${UX_WHEEL_HOVER_FOCUS_OUT:-$ROOT_DIR/tmp/ux_wheel_hover_focus_independent}"
BIN_OUT="$OUT_DIR/bin"
ROUTE="$ROOT_DIR/misc/editor/ux/wheel_route.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

fail() {
  echo "[ux wheel_hover_focus_independent] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$ROUTE" ] || fail "missing wheel_route.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"

grep -q 'export fn editor_wheel_hover_target' "$ROUTE" || fail "missing editor_wheel_hover_target"
# Helper must not take / branch on editor_focused (Decision sabotage).
if grep -q 'editor_focused' "$ROUTE"; then
  fail "wheel_route must not reference editor_focused"
fi

grep -q "from './ux/wheel_route'" "$DEMO" || fail "demo_live missing wheel_route import"
grep -q 'editor_wheel_hover_target(' "$DEMO" || fail "demo_live missing editor_wheel_hover_target("

wheel_block="$(
  awk '/const scroll_ticks = f64_to_i32\(pending_scroll_y/,/visible = visible_line_range\(line_count, app\.ux\.scroll_offset_y/' "$DEMO"
)"
[ -n "$wheel_block" ] || fail "wheel block not found"
if printf '%s\n' "$wheel_block" | grep -q 'editor_focused'; then
  fail "wheel block must not reference editor_focused"
fi
if ! printf '%s\n' "$wheel_block" | grep -q 'editor_wheel_hover_target('; then
  fail "wheel block missing editor_wheel_hover_target("
fi
if ! printf '%s\n' "$wheel_block" | grep -q 'editor_app_wheel_scroll('; then
  fail "wheel block missing editor_app_wheel_scroll"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
[ "$status" -eq 0 ] || fail "scenario exit $status"
printf '%s\n' "$output" | grep -q 'ux_ok wheel_hover_focus_independent' \
  || fail "missing ux_ok wheel_hover_focus_independent"

echo "[ux wheel_hover_focus_independent] ok" >&2
