#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107n — Red: live vs scenario tab_strip_height diverge.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_probe_geometry_matches_live.sh +
# ux_scenarios/probe_geometry_matches_live.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROBE="$ROOT_DIR/misc/editor/ux/probe.mlc"
APP_STATE="$ROOT_DIR/misc/editor/app/state.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_probe_geometry_matches_live.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/probe_geometry_matches_live.mlc"

fail() {
  echo "[ux probe_geometry_matches_live_red] FAIL: $1" >&2
  exit 1
}

[ -f "$PROBE" ] || fail "missing $PROBE"
[ -f "$APP_STATE" ] || fail "missing $APP_STATE"

if ! grep -q 'export fn editor_ux_state_new(' "$PROBE"; then
  fail "missing editor_ux_state_new"
fi
if ! grep -q 'export fn editor_app_state_new(' "$APP_STATE"; then
  fail "missing editor_app_state_new"
fi

# Red: ux constructor still hardcodes scenario strip height.
if ! grep -q 'tab_strip_height: 28' "$PROBE"; then
  fail "editor_ux_state_new no longer hardcodes tab_strip_height: 28 (expected today's gap)"
fi

# Red: live path still forces 0 after/instead of a shared helper.
if ! grep -q 'tab_strip_height: 0' "$APP_STATE"; then
  fail "editor_app_state_new no longer forces tab_strip_height: 0 (expected today's gap)"
fi

# Red: shared live helper / explicit constructor param not shipped yet.
if grep -q 'editor_tab_strip_height_live' "$PROBE" "$APP_STATE"; then
  fail "editor_tab_strip_height_live already present (expected gap until Green)"
fi
if grep -Eq 'editor_ux_state_new\([^)]*tab_strip_height' "$PROBE"; then
  fail "editor_ux_state_new already takes tab_strip_height (expected gap until Green)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "tab_strip_height live 0 vs scenario 28 (false-green geometry) (Red expected)"
