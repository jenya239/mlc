#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108b — Red: chrome-only still rebuilds text layer.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_hover_no_text_layer_rebuild.sh + scenario + retain/replay.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_hover_no_text_layer_rebuild.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/hover_no_text_layer_rebuild.mlc"

fail() {
  echo "[ux hover_no_text_layer_rebuild_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PERF" ] || fail "missing $PERF"

# Sanity: counter + layout_skip exist (from §108a / §106).
if ! grep -q 'text_layer_rebuild_count' "$PERF"; then
  fail "perf missing text_layer_rebuild_count (unexpected drift)"
fi
if ! grep -q 'layout_skip' "$DEMO"; then
  fail "demo_live missing layout_skip (unexpected drift)"
fi

# Red: no retained text+gutter batch / replay path yet.
if grep -Eq 'retained_text_lines|text_layer_retain|replay_text_layer|retained_editor_lines' "$DEMO"; then
  fail "text-layer retain/replay already present (expected gap until Green)"
fi

# Red: every paint still allocates a fresh editor_lines list (no retain).
if ! grep -q 'let mut editor_lines: \[StaticTextLine\] = \[\]' "$DEMO"; then
  fail "demo_live no longer rebuilds editor_lines each paint (unexpected drift)"
fi

# Red: editor glyph draw is not gated by a retain/replay skip.
draw_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("let mut editor_lines: [StaticTextLine] = []")
    abort "missing editor_lines rebuild" if start_at.nil?
    draw_at = source.index("static_text_draw_lines_colored(", start_at)
    abort "missing editor static_text_draw after editor_lines" if draw_at.nil?
    print source[start_at...draw_at]
  ' "$DEMO"
)"
if printf '%s\n' "$draw_block" | grep -Eq 'retained_text|replay_text|text_layer_retain'; then
  fail "editor_lines path already retain-gated (expected gap until Green)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "chrome-only still rebuilds text+gutter layer each paint (Red expected)"
