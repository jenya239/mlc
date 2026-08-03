#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107p — single toolbar label+command table.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
CHROME="$ROOT_DIR/misc/editor/app/chrome.mlc"
BUS="$ROOT_DIR/misc/editor/commands/bus.mlc"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/tests/toolbar_table_unit.mlc"
OUT_DIR="${UX_TOOLBAR_TABLE_OUT:-$ROOT_DIR/tmp/ux_toolbar_table}"
BIN_OUT="$OUT_DIR/bin"

fail() {
  echo "[ux toolbar_table_single_source] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$CHROME" ] || fail "missing $CHROME"
[ -f "$BUS" ] || fail "missing $BUS"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"

if ! grep -q 'export type ToolbarEntry' "$CHROME"; then
  fail "ToolbarEntry missing in chrome.mlc"
fi
if ! grep -q 'CmdLoadSession' "$BUS"; then
  fail "CmdLoadSession missing in bus.mlc"
fi
if ! grep -q 'CmdLoadSession' "$CHROME"; then
  fail "CmdLoadSession missing in chrome.mlc"
fi
if grep -q 'while tool < 10' "$DEMO"; then
  fail "demo_live still has while tool < 10"
fi
if grep -q 'toolbar_button_rect(10,' "$DEMO"; then
  fail "demo_live still special-cases toolbar index 10"
fi
if grep -qE 'tools = \[' "$DEMO"; then
  fail "demo_live still has local tools = [ label array"
fi
if ! grep -q 'editor_app_toolbar_label_at' "$DEMO"; then
  fail "demo_live does not draw via editor_app_toolbar_label_at"
fi
if ! grep -q 'editor_app_toolbar_count' "$DEMO"; then
  fail "demo_live does not hit-test via editor_app_toolbar_count"
fi
if ! grep -q '| CmdLoadSession =>' "$DEMO"; then
  fail "demo_live missing CmdLoadSession dispatch arm"
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
if [ "$status" -ne 0 ]; then
  fail "toolbar_table_unit exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q '\[ux\] toolbar_table_unit ok'; then
  fail "missing toolbar_table_unit ok"
fi

echo "ux_ok toolbar_table_single_source"
echo "[ux toolbar_table_single_source] ok" >&2
