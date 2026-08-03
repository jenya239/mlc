#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107p — Red: toolbar hit-test/draw/command tables diverge.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_toolbar_table_single_source.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
CHROME="$ROOT_DIR/misc/editor/app/chrome.mlc"
BUS="$ROOT_DIR/misc/editor/commands/bus.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_toolbar_table_single_source.sh"

fail() {
  echo "[ux toolbar_table_single_source_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$CHROME" ] || fail "missing $CHROME"
[ -f "$BUS" ] || fail "missing $BUS"

# Red: hit-test still truncated + magic SessLd index.
if ! grep -q 'while tool < 10' "$DEMO"; then
  fail "demo_live no longer has while tool < 10 (expected today's gap)"
fi
if ! grep -q 'toolbar_button_rect(10,' "$DEMO"; then
  fail "demo_live no longer special-cases toolbar index 10 (expected today's gap)"
fi

# Red: local draw label array still present.
if ! grep -q 'SessLd' "$DEMO"; then
  fail "demo_live missing SessLd label (expected today's gap)"
fi
if ! grep -q 'tools = \[' "$DEMO"; then
  fail "demo_live missing local tools = [ label array (expected today's gap)"
fi

# Red: command map does not cover index 10 / no CmdLoadSession yet.
if grep -q 'CmdLoadSession' "$BUS" "$CHROME"; then
  fail "CmdLoadSession already present (expected gap until Green)"
fi
if grep -q 'ToolbarEntry' "$CHROME"; then
  fail "ToolbarEntry already present (expected gap until Green)"
fi

# Red: green gate artifact must be absent.
if [ -f "$STABLE" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "toolbar hit-test <10 + magic 10 vs 11 draw labels / command_at 0..9 (Red expected)"
