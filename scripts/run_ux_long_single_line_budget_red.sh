#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107h — Red: long-line shape path still uncapped.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_long_single_line_budget.sh +
# ux_scenarios/long_single_line_budget.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
WRAP="$ROOT_DIR/misc/editor/layout/word_wrap.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_long_single_line_budget.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/long_single_line_budget.mlc"

fail() {
  echo "[ux long_single_line_budget_red] FAIL: $1" >&2
  exit 1
}

[ -f "$WRAP" ] || fail "missing $WRAP"

advances_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("fn line_codepoint_advances_px(")
    abort "missing line_codepoint_advances_px" if start_at.nil?
    end_at = source.index("\nfn ", start_at + 1)
    abort "missing next fn" if end_at.nil?
    print source[start_at...end_at]
  ' "$WRAP"
)"

# Red: no byte / segment caps yet.
if printf '%s\n' "$advances_block" | grep -Eq 'SHAPE_LINE_BYTE_CAP|SHAPE_SEGMENT_CODEPOINT_MAX'; then
  fail "shape caps already present in line_codepoint_advances_px (expected gap until Green)"
fi

# Red: mismatch path still shapes each codepoint alone.
if ! printf '%s\n' "$advances_block" | grep -q 'shape each codepoint alone'; then
  fail "per-codepoint mismatch fallback comment gone (unexpected drift)"
fi
if ! printf '%s\n' "$advances_block" | grep -q 'text_shaping_shape(state, font_path, pixel_size, piece)'; then
  fail "per-codepoint text_shaping_shape(piece) missing (expected today's gap)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "long single-line shape path still uncapped (Red expected)"
