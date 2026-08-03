#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108a — Red: stable-hit hover still forces a frame.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_hover_stable_hit_no_content_frame.sh + scenario + counters.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
FRAME_INPUT="$ROOT_DIR/misc/editor/app/frame_input.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_hover_stable_hit_no_content_frame.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/hover_stable_hit_no_content_frame.mlc"

fail() {
  echo "[ux hover_stable_hit_no_content_frame_red] FAIL: $1" >&2
  exit 1
}

[ -f "$FRAME_INPUT" ] || fail "missing $FRAME_INPUT"
[ -f "$PERF" ] || fail "missing $PERF"

# Sanity: today's pointer_dirty path exists.
if ! grep -q 'pointer_dirty' "$FRAME_INPUT"; then
  fail "frame_input missing pointer_dirty (unexpected drift)"
fi

# Red: mouse move still unconditionally sets pointer_dirty (no hit-style id gate).
mouse_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("if input.mouse_x != last_mouse_x || input.mouse_y != last_mouse_y then")
    abort "missing mouse-move dirty block" if start_at.nil?
    window_end = source.index("\n  end\n  if input.mouse_down", start_at)
    abort "missing mouse-move block end" if window_end.nil?
    print source[start_at...window_end]
  ' "$FRAME_INPUT"
)"
if ! printf '%s\n' "$mouse_block" | grep -q 'pointer_dirty = 1'; then
  fail "mouse-move block no longer sets pointer_dirty = 1 (unexpected drift)"
fi
if printf '%s\n' "$mouse_block" | grep -Eq 'hit_style|chrome_hit|chrome_dirty'; then
  fail "mouse-move already hit-style gated (expected gap until Green)"
fi

# Red: overlay / context menu still force content_dirty (B7 residual).
if ! grep -q 'if context_menu_visible then' "$FRAME_INPUT"; then
  fail "missing context_menu_visible content_dirty branch (unexpected drift)"
fi
context_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("if context_menu_visible then")
    abort "missing context_menu_visible" if start_at.nil?
    window_end = source.index("\n  end\n  if overlay_visible", start_at)
    abort "missing context_menu block end" if window_end.nil?
    print source[start_at...window_end]
  ' "$FRAME_INPUT"
)"
if ! printf '%s\n' "$context_block" | grep -q 'content_dirty = 1'; then
  fail "context_menu no longer raises content_dirty (expected today's gap)"
fi

# Red: rebuild counters not wired yet.
if grep -Eq 'content_rebuild_count|text_layer_rebuild_count' "$PERF" "$FRAME_INPUT"; then
  fail "rebuild counters already present (expected gap until Green)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "stable-hit hover still bumps content/text rebuild (pointer_dirty always-on; Red expected)"
