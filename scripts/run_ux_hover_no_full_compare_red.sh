#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107i — Red: spans tick still outside layout_skip.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_hover_no_full_compare.sh +
# ux_scenarios/hover_no_full_compare.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_hover_no_full_compare.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/hover_no_full_compare.mlc"

fail() {
  echo "[ux hover_no_full_compare_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"

# Sanity: layout_skip path exists for wrap ticks.
if ! grep -q 'layout_skip' "$DEMO"; then
  fail "demo_live missing layout_skip (unexpected drift)"
fi
if ! grep -q 'frame_layout_tick_spans' "$DEMO"; then
  fail "demo_live missing frame_layout_tick_spans (unexpected drift)"
fi

# Red: paint spans tick is not gated by layout_skip.
spans_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("frame_layout = frame_layout_tick_spans(")
    abort "missing frame_layout_tick_spans call" if start_at.nil?
    # Take a window from the preceding "if" through the matching end.
    window_start = source.rindex("\n    if ", start_at)
    abort "missing enclosing if" if window_start.nil?
    window_end = source.index("\n    end\n", start_at)
    abort "missing enclosing end" if window_end.nil?
    print source[window_start...window_end]
  ' "$DEMO"
)"
if printf '%s\n' "$spans_block" | grep -q 'layout_skip'; then
  fail "frame_layout_tick_spans already gated by layout_skip (expected gap until Green)"
fi
if ! printf '%s\n' "$spans_block" | grep -q 'perf_skip_heavy'; then
  fail "spans tick block missing perf_skip_heavy (unexpected drift)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "spans tick still outside layout_skip (Red expected)"
