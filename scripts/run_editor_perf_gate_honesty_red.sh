#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109c — Red: gate honesty gap still open.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_gate_honesty.sh + rewrites hover L2 VISIBLE=1
# and PERF_FULL TOTAL_US_MAX ≤ measured×1.25 (kills default 20000000).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_gate_honesty.sh"
HOVER="$ROOT_DIR/scripts/run_ux_hover_cpu_budget.sh"
PERF_FULL="$ROOT_DIR/scripts/run_editor_demo_live_perf_full_smoke.sh"
IDLE="$ROOT_DIR/scripts/run_ux_idle_cpu_budget_stable.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_gate_honesty_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"
[ -f "$HOVER" ] || fail "missing run_ux_hover_cpu_budget.sh (unexpected drift)"
[ -f "$PERF_FULL" ] || fail "missing run_editor_demo_live_perf_full_smoke.sh (unexpected drift)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"

# Sanity: §109b wake authority retained.
grep -q 'MLC_GLFW_VISIBLE=1' "$WAKE" || fail "wake harness missing VISIBLE=1 (unexpected drift)"

# Red: green honesty harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_gate_honesty.sh already present (expected gap until Green)"
fi

# Red: hover L2 still false-green path (VISIBLE=0 + cpu=0 ceiling 10).
if ! grep -q 'export MLC_GLFW_VISIBLE=0' "$HOVER"; then
  fail "hover_cpu_budget no longer sets VISIBLE=0 (expected false-green until Green)"
fi
if ! grep -q 'COMMITTED_HOVER_CPU_BUDGET_PERCENT=10' "$HOVER"; then
  fail "hover_cpu_budget missing COMMITTED_HOVER_CPU_BUDGET_PERCENT=10 (expected until Green)"
fi
if ! grep -q 'cpu_percent=0' "$HOVER"; then
  fail "hover_cpu_budget missing cpu=0 provenance comment (expected until Green)"
fi

# Red: PERF_FULL still loose 20e6 default.
if ! grep -q 'TOTAL_US_MAX=.*20000000' "$PERF_FULL"; then
  fail "PERF_FULL no longer defaults TOTAL_US_MAX to 20000000 (expected until Green)"
fi

# Red: idle gate header not yet honesty-labeled (optional until Green; require absence of still-over-text disclaimer).
if [ -f "$IDLE" ] && grep -qi 'must not be cited as still-over-text' "$IDLE"; then
  fail "idle gate already has still-over-text disclaimer (expected gap until Green)"
fi

# Red: no honesty measured table yet.
if grep -A8 '### Gate honesty (measured)' "$TRACK" | grep -Eq 'TOTAL_US_MAX|measured_total_us|[0-9]{6,}'; then
  fail "TRACK already has gate honesty measured numbers (expected gap until Green)"
fi

fail "no editor perf gate honesty (Red expected)"
