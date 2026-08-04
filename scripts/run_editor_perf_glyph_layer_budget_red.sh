#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109e — Red: no glyph-layer budget harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_glyph_layer_budget.sh + retained editor glyph batch
# (kills per-frame static_text_draw_lines_colored(editor_lines) + HarfBuzz).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
CONTENT_FRAME="$ROOT_DIR/scripts/run_editor_perf_content_frame_budget.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_glyph_layer_budget_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$CONTENT_FRAME" ] || fail "missing content-frame budget (unexpected drift — §109d required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green glyph-layer budget harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_glyph_layer_budget.sh already present (expected gap until Green)"
fi

# Red: editor_glyph_shape_calls not wired yet (§109e Green).
if grep -q 'editor_glyph_shape_calls' "$PERF"; then
  fail "perf already has editor_glyph_shape_calls (expected gap until Green)"
fi

# Red: editor text still shaped every content frame via colored-lines path.
if ! grep -q 'static_text_draw_lines_colored(' "$DEMO"; then
  fail "demo_live missing static_text_draw_lines_colored (unexpected drift)"
fi
if ! awk '
  /static_text_draw_lines_colored\(/ { in_call=1; buf=$0 }
  in_call && !/static_text_draw_lines_colored\(/ { buf=buf $0 }
  in_call && /\)/ {
    if (buf ~ /editor_lines/) found=1
    in_call=0
    buf=""
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live no longer draws editor_lines via static_text_draw_lines_colored (expected until Green)"
fi

# Red: no separate editor_glyph_batch wire yet.
if grep -q 'editor_glyph_batch' "$DEMO"; then
  fail "demo_live already has editor_glyph_batch (expected gap until Green)"
fi

# Red: no post-cut glyph-layer measured numbers yet.
if grep -A20 '## §109e\|### Glyph.layer\|### Editor glyph' "$TRACK" 2>/dev/null | \
  grep -Eq 'scroll_cpu_percent \| [0-9]+|editor_glyph_shape|post.cut|Green \(measured\)'; then
  # Allow Decision text mentioning counters; fail only on measured-result tables.
  if grep -A30 '### Glyph.layer Green\|### Editor glyph Green\|### Glyph-layer Green' "$TRACK" 2>/dev/null | \
    grep -Eq '[0-9]{2,}'; then
    fail "TRACK already has glyph-layer post-cut measured numbers (expected gap until Green)"
  fi
fi

fail "no editor perf glyph-layer budget harness (Red expected)"
