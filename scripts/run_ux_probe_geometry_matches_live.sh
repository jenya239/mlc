#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107n — probe geometry matches live tab_strip_height.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/probe_geometry_matches_live.mlc"
OUT_DIR="${UX_PROBE_GEOMETRY_OUT:-$ROOT_DIR/tmp/ux_probe_geometry_matches_live}"
BIN_OUT="$OUT_DIR/bin"
PROBE="$ROOT_DIR/misc/editor/ux/probe.mlc"
APP_STATE="$ROOT_DIR/misc/editor/app/state.mlc"

fail() {
  echo "[ux probe_geometry_matches_live] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$PROBE" ] || fail "missing $PROBE"
[ -f "$APP_STATE" ] || fail "missing $APP_STATE"

if ! grep -q 'export fn editor_tab_strip_height_live' "$PROBE"; then
  fail "editor_tab_strip_height_live missing"
fi
if ! grep -q 'tab_strip_height: i32' "$PROBE"; then
  fail "editor_ux_state_new missing tab_strip_height param"
fi
if grep -q 'tab_strip_height: 28' "$PROBE"; then
  fail "editor_ux_state_new still hardcodes tab_strip_height: 28"
fi
if ! grep -q 'editor_tab_strip_height_live()' "$APP_STATE"; then
  fail "app state missing editor_tab_strip_height_live()"
fi
if grep -q 'tab_strip_height: 0' "$APP_STATE"; then
  fail "app state still assigns tab_strip_height: 0 literally"
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
  fail "binary exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok probe_geometry_matches_live'; then
  fail "missing ux_ok probe_geometry_matches_live"
fi
echo "[ux probe_geometry_matches_live] ok" >&2
