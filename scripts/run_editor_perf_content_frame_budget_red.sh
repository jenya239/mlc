#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109d — Red: no content-frame budget harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_content_frame_budget.sh + retained minimap glyph batch
# (kills per-frame static_text_draw_lines_colored(minimap_lines) + HarfBuzz).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_content_frame_budget.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
PERF_FULL="$ROOT_DIR/scripts/run_editor_demo_live_perf_full_smoke.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_content_frame_budget_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$STATIC_TEXT" ] || fail "missing static_text.mlc (unexpected drift)"
[ -f "$PERF_FULL" ] || fail "missing PERF_FULL smoke (unexpected drift — §109c required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green content-frame budget harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_content_frame_budget.sh already present (expected gap until Green)"
fi

# Sanity: §109c ceiling was the standing PERF_FULL gate until §109d Green rewrites.
# After Green, this red script is only expected to fail on "already present" above.
if grep -q 'TOTAL_US_MAX=.*16357201' "$PERF_FULL"; then
  :
elif grep -q 'glyph_shape_calls' "$PERF"; then
  fail "green markers present without green harness file (unexpected drift)"
fi

# Red: glyph shape/batch counters not wired yet (§109d Green).
if grep -q 'glyph_shape_calls' "$PERF"; then
  fail "perf already has glyph_shape_calls (expected gap until Green)"
fi
if grep -q 'glyph_batch_draw_calls' "$PERF"; then
  fail "perf already has glyph_batch_draw_calls (expected gap until Green)"
fi

# Red: minimap still shaped every content frame via colored-lines path.
if ! grep -q 'static_text_draw_lines_colored(' "$DEMO"; then
  fail "demo_live missing static_text_draw_lines_colored (unexpected drift)"
fi
if ! grep -n 'minimap_lines' "$DEMO" | grep -q 'static_text_draw_lines_colored\|minimap_drawn'; then
  # Require the live call site that draws minimap_lines (Decision sabotage target).
  if ! awk '
    /static_text_draw_lines_colored\(/ { in_call=1 }
    in_call && /minimap_lines/ { found=1 }
    in_call && /\)/ { in_call=0 }
    END { exit(found ? 0 : 1) }
  ' "$DEMO"; then
    fail "demo_live no longer draws minimap_lines via static_text_draw_lines_colored (expected until Green)"
  fi
fi

# Red: no post-cut content-frame measured ceiling rewrite yet.
if grep -A12 '### Content.frame\|### Dominance (measured)' "$TRACK" 2>/dev/null | \
  grep -Eq 'TOTAL_US_MAX \| [0-9]+|post.cut|after cut'; then
  fail "TRACK already has content-frame post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf content-frame budget harness (Red expected)"
