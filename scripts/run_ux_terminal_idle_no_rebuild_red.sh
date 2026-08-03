#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107k — Red: terminal grid rebuilt every frame; damage unused.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_terminal_idle_no_rebuild.sh +
# ux_scenarios/terminal_idle_no_rebuild.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PANEL="$ROOT_DIR/misc/editor/app/terminal_panel.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_terminal_idle_no_rebuild.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/terminal_idle_no_rebuild.mlc"

fail() {
  echo "[ux terminal_idle_no_rebuild_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PANEL" ] || fail "missing $PANEL"

# Red: paint still rebuilds grid unconditionally.
if ! grep -q 'terminal_grid_text_lines(' "$DEMO"; then
  fail "demo_live missing terminal_grid_text_lines (unexpected drift)"
fi
if ! grep -q 'terminal_grid_draw_backgrounds(' "$DEMO"; then
  fail "demo_live missing terminal_grid_draw_backgrounds (unexpected drift)"
fi

# Red: product code does not consume damage_count for invalidation.
if grep -Eq 'vterm_damage_count|vterm_terminal_damage_count' "$DEMO" "$PANEL"; then
  fail "damage_count already consumed in demo/panel (expected gap until Green)"
fi

# Red: no paint cache fields on the session yet.
if grep -Eq 'cached_text_lines|grid_rebuild_count|cached_damage_count' "$PANEL"; then
  fail "terminal grid cache fields already present (expected gap until Green)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "terminal grid still rebuilt every frame without vterm_damage_count (Red expected)"
