#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107k — damage-keyed terminal grid cache (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/terminal_idle_no_rebuild.mlc"
OUT_DIR="${UX_TERMINAL_IDLE_NO_REBUILD_OUT:-$ROOT_DIR/tmp/ux_terminal_idle_no_rebuild}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PANEL="$ROOT_DIR/misc/editor/app/terminal_panel.mlc"

fail() {
  echo "[ux terminal_idle_no_rebuild] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PANEL" ] || fail "missing $PANEL"

# Arch: paint path consults cache tick / damage, not bare unconditional rebuild.
if ! grep -q 'terminal_panel_session_grid_cache_tick' "$DEMO"; then
  fail "demo_live missing terminal_panel_session_grid_cache_tick"
fi
if ! grep -q 'terminal_grid_draw_cached_backgrounds' "$DEMO"; then
  fail "demo_live missing terminal_grid_draw_cached_backgrounds"
fi
if ! grep -q 'cached_text_lines' "$DEMO"; then
  fail "demo_live not drawing from cached_text_lines"
fi
if grep -q 'terminal_grid_text_lines(' "$DEMO"; then
  fail "demo_live still calls terminal_grid_text_lines unconditionally"
fi
if grep -q 'terminal_grid_draw_backgrounds(' "$DEMO"; then
  fail "demo_live still calls terminal_grid_draw_backgrounds unconditionally"
fi
if ! grep -q 'vterm_terminal_damage_count' "$PANEL"; then
  fail "terminal_panel missing vterm_terminal_damage_count"
fi
if ! grep -q 'grid_rebuild_count' "$PANEL"; then
  fail "terminal_panel missing grid_rebuild_count"
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
if ! printf '%s\n' "$output" | grep -q 'ux_ok terminal_idle_no_rebuild'; then
  fail "missing ux_ok terminal_idle_no_rebuild"
fi
echo "[ux terminal_idle_no_rebuild] ok" >&2
