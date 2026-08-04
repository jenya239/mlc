#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109h — Red: no chrome-tree-visible harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_chrome_tree_visible.sh + visible-range tree walks
# + tree_row_visit_count (+ toolbar hover into chrome_layer).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_chrome_tree_visible.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/chrome_tree_visible.mlc"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
SNAPSHOT="$ROOT_DIR/scripts/run_editor_perf_snapshot_edit_coverage.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_chrome_tree_visible_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$SNAPSHOT" ] || fail "missing snapshot-edit harness (unexpected drift — §109g required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green chrome-tree-visible harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_chrome_tree_visible.sh already present (expected gap until Green)"
fi

# Red: L1 scenario not present yet.
if [ -f "$SCENARIO" ]; then
  fail "chrome_tree_visible.mlc already present (expected gap until Green)"
fi

# Red: tree_row_visit_count counter not wired yet (§109h Green).
if grep -q 'tree_row_visit_count' "$PERF" "$DEMO" 2>/dev/null; then
  fail "tree_row_visit_count already present (expected gap until Green)"
fi

# Red: Decision pre-cut — full-length tree hover + glyph loops still present.
if ! grep -q 'while hover_row_index < draw_folder_rows.length()' "$DEMO"; then
  fail "demo_live no longer full-length tree hover loop (expected until Green)"
fi
if ! grep -q 'while row_index < draw_tree_rows.length()' "$DEMO"; then
  fail "demo_live no longer full-length tree glyph loop (expected until Green)"
fi

# Red: toolbar hover still outside chrome_layer (pre-cut walk of all slots).
if ! grep -q 'while hover_tool_index < hover_toolbar_count' "$DEMO"; then
  fail "demo_live no longer walks all toolbar hover slots (expected until Green)"
fi

# Red: no post-cut chrome-tree measured numbers yet.
if grep -A30 '### Chrome-tree\|### Chrome.tree Green\|### §109h Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'tree_row_visit|visible_slots \| [0-9]+'; then
  fail "TRACK already has chrome-tree post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf chrome-tree-visible harness (Red expected)"
