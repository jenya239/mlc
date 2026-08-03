#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107j — Red: visible-row collect still multi-call per frame.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_visible_rows_single_collect.sh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_visible_rows_single_collect.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/visible_rows_single_collect.mlc"

fail() {
  echo "[ux visible_rows_single_collect_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PERF" ] || fail "missing $PERF"

collect_count="$(
  grep -c 'collect_visible_visual_rows_pixel_budget_cached(' "$DEMO" || true
)"
if [ "$collect_count" -lt 3 ]; then
  fail "demo_live collect sites=$collect_count (expected ≥3 until Green)"
fi
if [ "$collect_count" -eq 1 ]; then
  fail "demo_live already single-collect (expected gap until Green)"
fi

# Red: perf counter for visible collects not wired yet.
if grep -q 'visible_collect_count' "$PERF"; then
  fail "visible_collect_count already present in perf.mlc (expected gap until Green)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "visible-row collect still multi-call per frame (Red expected)"
