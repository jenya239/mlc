#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109a — Red: no visible dogfood baseline harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_dogfood_baseline.sh + MLC_EDITOR_PERF_DOGFOOD probe
# in demo_live + .tmp/.../report.txt numbers pasted into the track.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
REPORT_DIR="$ROOT_DIR/.tmp/editor_perf_dogfood_baseline"

fail() {
  echo "[editor_perf_dogfood_baseline_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Sanity: Decision open path hook already exists (reuse, not invent).
if ! grep -q 'MLC_EDITOR_PERF_OPEN' "$DEMO"; then
  fail "demo_live missing MLC_EDITOR_PERF_OPEN (Decision prerequisite)"
fi

# Red: green harness must be absent until Green step.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_dogfood_baseline.sh already present (expected gap until Green)"
fi

# Red: internal phase driver not wired yet.
if grep -q 'MLC_EDITOR_PERF_DOGFOOD' "$DEMO"; then
  fail "demo_live already has MLC_EDITOR_PERF_DOGFOOD (expected gap until Green)"
fi

# Red: no committed measured baseline table yet.
if grep -A2 '### Baseline (measured)' "$TRACK" | grep -Eq 'idle_away_cpu_percent|[0-9]+%'; then
  fail "TRACK already has measured baseline numbers (expected gap until Green)"
fi

# Red: report path should not already claim a green run in-tree.
if [ -f "$REPORT_DIR/report.txt" ]; then
  fail "report.txt already present under .tmp (expected gap until Green)"
fi

fail "no editor perf dogfood baseline harness (Red expected)"
