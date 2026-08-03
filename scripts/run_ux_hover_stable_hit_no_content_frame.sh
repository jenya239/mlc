#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108a — stable-hit hover skips content/text rebuild.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/hover_stable_hit_no_content_frame.mlc"
OUT_DIR="${UX_HOVER_STABLE_HIT_OUT:-$ROOT_DIR/tmp/ux_hover_stable_hit_no_content_frame}"
BIN_OUT="$OUT_DIR/bin"
FRAME_INPUT="$ROOT_DIR/misc/editor/app/frame_input.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
CHROME_HIT="$ROOT_DIR/misc/editor/ux/chrome_hit.mlc"

fail() {
  echo "[ux hover_stable_hit_no_content_frame] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$CHROME_HIT" ] || fail "missing chrome_hit.mlc"
[ -f "$FRAME_INPUT" ] || fail "missing frame_input.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"

grep -q 'content_rebuild_count' "$PERF" || fail "perf missing content_rebuild_count"
grep -q 'text_layer_rebuild_count' "$PERF" || fail "perf missing text_layer_rebuild_count"
grep -q 'chrome_dirty' "$FRAME_INPUT" || fail "frame_input missing chrome_dirty"
grep -q 'editor_ux_chrome_hit_pointer_plan' "$DEMO" || fail "demo_live missing hit pointer plan"
grep -q 'last_chrome_hit_id' "$DEMO" || fail "demo_live missing last_chrome_hit_id"

# Pointer move must not raise content_dirty (Decision sabotage).
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
if printf '%s\n' "$mouse_block" | grep -q 'content_dirty = 1'; then
  fail "mouse-move raises content_dirty"
fi
if ! printf '%s\n' "$mouse_block" | grep -q 'pointer_dirty = 1'; then
  fail "mouse-move missing pointer_dirty = 1"
fi

# Overlay / context menu must not raise content_dirty.
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
if printf '%s\n' "$context_block" | grep -q 'content_dirty = 1'; then
  fail "context_menu still raises content_dirty"
fi
if ! printf '%s\n' "$context_block" | grep -q 'chrome_dirty = 1'; then
  fail "context_menu does not raise chrome_dirty"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  fail "binary exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok hover_stable_hit_no_content_frame'; then
  fail "missing ux_ok"
fi
echo "[ux hover_stable_hit_no_content_frame] ok" >&2
