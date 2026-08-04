#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109k — Red: no composite dogfood-gate harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_dogfood_gate.sh composing honesty→wake→dogfood
# ceilings→glyph→tree→minimap→startup (×2 before Critic).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
HONESTY="$ROOT_DIR/scripts/run_editor_perf_gate_honesty.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
TREE="$ROOT_DIR/scripts/run_editor_perf_chrome_tree_visible.sh"
MINIMAP="$ROOT_DIR/scripts/run_editor_perf_minimap_sample.sh"
STARTUP="$ROOT_DIR/scripts/run_editor_perf_startup_open.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_dogfood_gate_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"
[ -f "$HONESTY" ] || fail "missing gate honesty (unexpected drift — §109c required)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$GLYPH" ] || fail "missing glyph-layer budget (unexpected drift — §109e required)"
[ -f "$TREE" ] || fail "missing chrome-tree harness (unexpected drift — §109h required)"
[ -f "$MINIMAP" ] || fail "missing minimap-sample harness (unexpected drift — §109i required)"
[ -f "$STARTUP" ] || fail "missing startup-open harness (unexpected drift — §109j required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green composite gate absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_dogfood_gate.sh already present (expected gap until Green)"
fi

# Red: dogfood baseline still measure-only (no epic CPU ceiling fails).
# Do not match summary `echo … >&2` (`.*>` would hit the redirect).
if grep -Eq 'idle_away_cpu_percent.*-(gt|ge|le|lt)|still_over_text_cpu_percent.*-(gt|ge|le|lt)|scroll_cpu_percent.*-(gt|ge|le|lt)|type_stall_ms.*-(gt|ge|le|lt)' \
  "$DOGFOOD"; then
  fail "dogfood baseline already compares epic CPU/stall ceilings (expected measure-only until Green)"
fi
if grep -Eq 'FAIL:.*(scroll_cpu|still_over|idle_away|type_stall|jitter_cpu)' "$DOGFOOD"; then
  fail "dogfood baseline already has epic ceiling FAIL messages (expected until Green)"
fi
if grep -Eq 'IDLE_AWAY_CPU_MAX|STILL_OVER_CPU_MAX|SCROLL_CPU_MAX|TYPE_STALL_MS_MAX|JITTER_CPU_MAX' "$DOGFOOD"; then
  fail "dogfood baseline already has epic ceiling env knobs (expected until Green)"
fi

# Red: ux_gate still does not compose editor_perf dogfood suite.
if grep -q 'run_editor_perf_dogfood_gate\|run_editor_perf_dogfood_baseline\|run_editor_perf_wake_on_hover' \
  "$UX_GATE"; then
  fail "run_ux_gate already wires editor_perf dogfood members (Decision: not folded)"
fi

# Red: no §109k Green measured / ×2 paste yet.
if grep -A40 '### Dogfood.gate\|### §109k Green\|### Gate suite' "$TRACK" 2>/dev/null | \
  grep -Eq 'scroll_cpu_percent \| [0-9]+|×2|x2 run'; then
  fail "TRACK already has dogfood-gate post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf dogfood-gate harness (Red expected)"
