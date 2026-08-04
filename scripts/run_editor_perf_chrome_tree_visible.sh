#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109h — Green: visible-range tree + toolbar in chrome.
# Authority: L1 chrome_tree_visible + static wire; dogfood side scroll/stall.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/chrome_tree_visible.mlc"
OUT_DIR="${EDITOR_PERF_CHROME_TREE_OUT:-$ROOT_DIR/.tmp/editor_perf_chrome_tree_visible}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
FOLDER="$ROOT_DIR/misc/editor/ux/folder_panel.mlc"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
GLYPH="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_chrome_tree_visible] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing chrome_tree_visible.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"
[ -f "$PERF" ] || fail "missing perf.mlc"
[ -f "$FOLDER" ] || fail "missing folder_panel.mlc"
[ -f "$WAKE" ] || fail "missing wake harness (§109b)"
[ -f "$GLYPH" ] || fail "missing glyph-layer budget (§109e)"
[ -f "$TRACK" ] || fail "missing TRACK"

grep -q 'tree_row_visit_count' "$PERF" || fail "perf.mlc missing tree_row_visit_count"
grep -q 'folder_visible_row_range' "$FOLDER" || fail "folder_panel missing folder_visible_row_range"
grep -q 'folder_visible_row_range' "$DEMO" || fail "demo_live missing folder_visible_row_range"
grep -q 'editor_perf_note_tree_row_visits' "$DEMO" || fail "demo_live missing tree visit notes"
grep -q 'toolbar_hover' "$DEMO" || fail "demo_live missing toolbar_hover in chrome_layer"

# Must not full-length walk tree hover/glyph.
if grep -q 'while hover_row_index < draw_folder_rows.length()' "$DEMO"; then
  fail "demo_live still full-length tree hover loop"
fi
if grep -q 'while row_index < draw_tree_rows.length()' "$DEMO"; then
  fail "demo_live still full-length tree glyph loop"
fi
# Toolbar hover must not sit outside chrome_layer as post-flatten solid walk.
if grep -q 'while hover_tool_index < hover_toolbar_count' "$DEMO"; then
  # Allowed only before chrome_layer store (inside rebuild). Ensure solid_renderer_rect
  # is not used for the old post-flatten toolbar loop by requiring toolbar_hover ops.
  :
fi
# Old post-flatten pattern: solid_renderer_rect after flatten for tools — require
# no "hover_toolbar_count" after "editor_paint_ops_flatten(solid, width, height, chrome_layer_batch.ops)"
if awk '
  /editor_paint_ops_flatten\(solid, width, height, chrome_layer_batch\.ops\)/ { after=1 }
  after && /while hover_tool_index < hover_toolbar_count/ { found=1 }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live still walks toolbar hover after chrome_layer flatten"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
l2_output=$("$BIN_OUT" 2>&1)
l2_status=$?
set -e
printf '%s\n' "$l2_output" | tee "$OUT_DIR/l1.log"
[ "$l2_status" -eq 0 ] || fail "L1 binary exit=$l2_status"
printf '%s\n' "$l2_output" | grep -q 'ux_ok chrome_tree_visible' || \
  fail "missing ux_ok chrome_tree_visible"

# §109b still chrome_rebuild delta==0 (shortened samples via env if supported).
export EDITOR_PERF_WAKE_OUT="$OUT_DIR/wake"
set +e
wake_output="$(bash "$WAKE" 2>&1)"
wake_status=$?
set -e
printf '%s\n' "$wake_output" | tee "$OUT_DIR/wake.log"
[ "$wake_status" -eq 0 ] || fail "wake harness failed (exit=$wake_status)"
printf '%s\n' "$wake_output" | grep -q 'delta_chrome_rebuild_count=0' || \
  fail "wake chrome_rebuild delta not 0"

# §109e scroll non-regress.
export EDITOR_PERF_GLYPH_LAYER_OUT="$OUT_DIR/glyph_layer"
set +e
glyph_output="$(bash "$GLYPH" 2>&1)"
glyph_status=$?
set -e
printf '%s\n' "$glyph_output" | tee "$OUT_DIR/glyph_layer.log"
[ "$glyph_status" -eq 0 ] || fail "glyph-layer budget failed (exit=$glyph_status)"

scroll_cpu="$(
  if [ -f "$OUT_DIR/glyph_layer/report.txt" ]; then
    grep -E '^scroll_cpu_percent=' "$OUT_DIR/glyph_layer/report.txt" | cut -d= -f2 | tail -n1
  fi
)"
if [ -z "$scroll_cpu" ]; then
  scroll_cpu="$(printf '%s\n' "$glyph_output" | sed -n 's/.*\[editor_perf_glyph_layer_budget\] OK .*scroll_cpu=\([0-9][0-9]*\).*/\1/p' | tail -n1)"
fi
[ -n "$scroll_cpu" ] || fail "missing scroll_cpu_percent"
[ "$scroll_cpu" -le 60 ] || fail "scroll_cpu=$scroll_cpu > 60"

echo "l1=chrome_tree_visible"
echo "scroll_cpu_percent=$scroll_cpu"
echo "[editor_perf_chrome_tree_visible] OK scroll=$scroll_cpu%"
