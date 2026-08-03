#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108c — present_only caret must not rebuild chrome.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/present_only_caret_no_chrome_rebuild.mlc"
OUT_DIR="${UX_PRESENT_ONLY_CARET_OUT:-$ROOT_DIR/tmp/ux_present_only_caret_no_chrome_rebuild}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
BATCH="$ROOT_DIR/misc/editor/ux/chrome_layer_batch.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"

fail() {
  echo "[ux present_only_caret_no_chrome_rebuild] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$BATCH" ] || fail "missing chrome_layer_batch.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF" ] || fail "missing perf.mlc"

grep -q 'chrome_rebuild_count' "$PERF" || fail "perf missing chrome_rebuild_count"
grep -q 'editor_perf_note_chrome_rebuild' "$PERF" || fail "perf missing note_chrome_rebuild"
grep -q 'editor_ux_chrome_layer_batch_can_replay' "$BATCH" || fail "batch missing can_replay"
grep -q 'editor_ux_compose_present_layer_names' "$BATCH" || fail "batch missing compose order"
grep -q 'chrome_layer_batch' "$DEMO" || fail "demo_live missing chrome_layer_batch"
grep -q 'editor_ux_chrome_layer_batch_store' "$DEMO" || fail "demo_live missing chrome store"
grep -q 'editor_perf_note_chrome_rebuild' "$DEMO" || fail "demo_live missing note_chrome_rebuild"

# Decision sabotage: present_only caret path must not rebuild chrome.
ruby -e '
demo = File.read(ARGV[0])
start_at = demo.index("paint_dirty = caret_blink_paint_dirty(")
abort "missing caret present_only paint_dirty" if start_at.nil?
# End at the else that advances clock for non-idle frames.
window_end = demo.index("\n    else\n      clock_ms = clock_ms + 16", start_at)
abort "missing present_only block end" if window_end.nil?
block = demo[start_at...window_end]
if block.include?("editor_paint_ops_chrome_bands")
  abort "present_only calls chrome_bands (Decision sabotage)"
end
if block.include?("editor_perf_note_chrome_rebuild")
  abort "present_only notes chrome rebuild (Decision sabotage)"
end
if block.include?("editor_ux_chrome_layer_batch_store")
  abort "present_only stores chrome layer (Decision sabotage)"
end
' "$DEMO" || fail "present_only path rebuilds chrome"

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
if ! printf '%s\n' "$output" | grep -q 'ux_ok present_only_caret_no_chrome_rebuild'; then
  fail "missing ux_ok"
fi
echo "[ux present_only_caret_no_chrome_rebuild] ok" >&2
