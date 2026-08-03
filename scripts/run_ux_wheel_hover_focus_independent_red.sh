#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107r — Red: no behavioural L1 wheel_hover_focus gate.
# Excluded from run_ux_gate.sh (*_red.sh). Green adds
# scripts/run_ux_wheel_hover_focus_independent.sh + scenario.
# ARCH-LINT grep stable may already exist (EHA-19: that is the gap, not the fix).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
ARCH_LINT="$ROOT_DIR/scripts/run_ux_wheel_hover_focus_independent_stable.sh"
GREEN="$ROOT_DIR/scripts/run_ux_wheel_hover_focus_independent.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/wheel_hover_focus_independent.mlc"

fail() {
  echo "[ux wheel_hover_focus_independent_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"

# Sanity: arch-lint grep gate still present (today's false-green class).
[ -f "$ARCH_LINT" ] || fail "missing arch-lint stable (unexpected drift)"
if ! grep -q 'tree_hovered' "$ARCH_LINT"; then
  fail "arch-lint stable missing tree_hovered grep (unexpected drift)"
fi

# Red: behavioural L1 artifacts must be absent.
if [ -f "$GREEN" ]; then
  fail "green behavioural runner already present (expected gap until Green)"
fi
if [ -f "$SCENARIO" ]; then
  fail "behavioural scenario already present (expected gap until Green)"
fi

fail "no behavioural L1 wheel_hover_focus_independent gate (Red expected)"
