#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109i — Red: no minimap-sample harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_minimap_sample.sh + sample-to-height minimap_lines
# rebuild (editor_ux_minimap_sample_*).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_minimap_sample.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/minimap_sample.mlc"
CONTENT="$ROOT_DIR/scripts/run_editor_perf_content_frame_budget.sh"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_minimap_sample_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$MINIMAP" ] || fail "missing minimap.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$CONTENT" ] || fail "missing content-frame budget (unexpected drift — §109d required)"
[ -f "$GLYPH" ] || fail "missing glyph-layer budget (unexpected drift — §109e required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green minimap-sample harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_minimap_sample.sh already present (expected gap until Green)"
fi

# Red: L1 scenario not present yet.
if [ -f "$SCENARIO" ]; then
  fail "minimap_sample.mlc already present (expected gap until Green)"
fi

# Red: sample helpers not wired yet (§109i Green).
if grep -q 'editor_ux_minimap_sample_count\|editor_ux_minimap_sample_source_line' \
  "$MINIMAP" "$DEMO" 2>/dev/null; then
  fail "minimap sample helpers already present (expected gap until Green)"
fi

# Red: Decision pre-cut — full document-line minimap_lines rebuild still present.
if ! grep -q 'while map_line < line_index_line_count(line_index)' "$DEMO"; then
  fail "demo_live no longer full line_count minimap rebuild loop (expected until Green)"
fi

# Red: no post-cut minimap-sample measured numbers yet.
if grep -A30 '### Minimap.sample\|### Minimap-sample Green\|### §109i Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'sample_row|strip_height \| [0-9]+|minimap_sample_row_count'; then
  fail "TRACK already has minimap-sample post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf minimap-sample harness (Red expected)"
