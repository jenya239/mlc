#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109f — Red: no spans-visible-only harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_spans_visible_only.sh + visible-range
# frame_layout_tick_spans (kills full-buffer 0..byte_size() on type).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_spans_visible_only.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_spans_visible_only_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$GLYPH" ] || fail "missing glyph-layer budget (unexpected drift — §109e required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green spans-visible harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_spans_visible_only.sh already present (expected gap until Green)"
fi

# Red: full-buffer span tick still present (Decision pre-cut).
if ! grep -q 'frame_layout_tick_spans(' "$DEMO"; then
  fail "demo_live missing frame_layout_tick_spans (unexpected drift)"
fi
if ! awk '
  /frame_layout_tick_spans\(/ { in_call=1; buf=$0 }
  in_call && !/frame_layout_tick_spans\(/ { buf=buf $0 }
  in_call && /\)/ {
    if (buf ~ /0,/ && buf ~ /byte_size\(\)/) found=1
    in_call=0
    buf=""
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live no longer ticks spans with 0..byte_size() (expected until Green)"
fi

# Red: span_lex_bytes counter not wired yet (§109f Green).
if grep -q 'span_lex_bytes' "$PERF" "$DEMO" 2>/dev/null; then
  fail "span_lex_bytes already present (expected gap until Green)"
fi

# Red: no post-cut spans-visible measured numbers yet.
if grep -A30 '### Spans.visible\|### Spans-visible Green\|### §109f Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'type_stall_ms \| [0-9]+|span_lex_bytes \| [0-9]+|[0-9]{3,}'; then
  fail "TRACK already has spans-visible post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf spans-visible-only harness (Red expected)"
