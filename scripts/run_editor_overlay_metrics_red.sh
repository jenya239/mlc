#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110f — Red: no overlay-metrics harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_overlay_metrics.sh + env-gated paint-list HUD.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_overlay_metrics.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
PERF_OVERLAY="$ROOT_DIR/misc/editor/ux/perf_overlay.mlc"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
GLYPH_DAMAGE="$ROOT_DIR/scripts/run_editor_glyph_damage.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"

fail() {
  echo "[editor_overlay_metrics_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$GLYPH_DAMAGE" ] || fail "missing glyph-damage harness (unexpected drift — §110e required)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green overlay-metrics harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_overlay_metrics.sh already present (expected gap until Green)"
fi

# Red: no MLC_EDITOR_PERF_OVERLAY wire in demo_live.
if grep -q 'MLC_EDITOR_PERF_OVERLAY' "$DEMO"; then
  fail "demo_live already wires MLC_EDITOR_PERF_OVERLAY (expected gap until Green)"
fi

# Red: no overlay format helper module / exports yet.
if [ -f "$PERF_OVERLAY" ]; then
  fail "ux/perf_overlay.mlc already present (expected gap until Green)"
fi
if grep -Eq 'editor_perf_overlay_format|perf_overlay_format|editor_overlay_metrics' \
  "$PERF" "$DEMO" 2>/dev/null; then
  fail "sources already declare overlay format helpers (expected until Green)"
fi

# Red: ux_gate still does not wire overlay-metrics harness.
if grep -q 'run_editor_overlay_metrics\|overlay_metrics\|MLC_EDITOR_PERF_OVERLAY' "$UX_GATE"; then
  fail "run_ux_gate already wires overlay metrics (Decision: not folded)"
fi

# Red: no §110f Green measured paste yet.
if grep -A40 '### §110f Green measured\|Green measured (§110f)\|### Green measured (§110f)' "$TRACK" 2>/dev/null | \
  grep -Eq 'overlay_env=|overlay_text_ops=|dogfood_gate=ok'; then
  fail "TRACK already has overlay-metrics post-cut measured numbers (expected gap until Green)"
fi

fail "no editor overlay metrics harness (Red expected)"
