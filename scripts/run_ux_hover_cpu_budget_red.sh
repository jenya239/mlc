#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108d — Red: no hover-under-pointer CPU/rebuild gate.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_hover_cpu_budget.sh + scenario (+ L2 ceiling after measure).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
IDLE="$ROOT_DIR/scripts/run_ux_idle_cpu_budget_stable.sh"
STABLE="$ROOT_DIR/scripts/run_ux_hover_cpu_budget.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/hover_cpu_budget.mlc"

fail() {
  echo "[ux hover_cpu_budget_red] FAIL: $1" >&2
  exit 1
}

[ -f "$PERF" ] || fail "missing $PERF"
[ -f "$IDLE" ] || fail "missing idle_cpu_budget_stable (unexpected drift)"

# Sanity: §108a–c counters exist (retain baseline).
if ! grep -q 'content_rebuild_count' "$PERF"; then
  fail "perf missing content_rebuild_count (unexpected drift)"
fi
if ! grep -q 'text_layer_rebuild_count' "$PERF"; then
  fail "perf missing text_layer_rebuild_count (unexpected drift)"
fi
if ! grep -q 'chrome_rebuild_count' "$PERF"; then
  fail "perf missing chrome_rebuild_count (unexpected drift)"
fi

# Red: idle gate still has no pointer-motion path (gap §108d covers).
if grep -Eq 'pointer_dirty|hover_jitter|HOVER_CPU|mouse_x' "$IDLE"; then
  fail "idle_cpu_budget_stable already samples under pointer (unexpected drift)"
fi

# Red: no L2 hover ceiling written yet (honesty: measure at Green only).
if grep -R -n --include='*.sh' 'HOVER_CPU_BUDGET_PERCENT' "$ROOT_DIR/scripts" 2>/dev/null \
  | grep -v '_red\.sh:' | grep -q .; then
  fail "HOVER_CPU_BUDGET_PERCENT already present (expected gap until Green measure)"
fi

# Red: green L1 gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green hover_cpu_budget gate already present (expected gap until Green)"
fi

fail "no hover-under-pointer CPU/rebuild gate (Red expected)"
