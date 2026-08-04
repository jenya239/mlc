#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109g — Red: no snapshot-edit coverage harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_snapshot_edit_coverage.sh + wires
# paste/cut/backspace/newline through frame_layout_tick_snapshot_edit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_snapshot_edit_coverage.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
EDIT_SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/edit_no_full_flatten.mlc"
SIBLING="$ROOT_DIR/misc/editor/ux_scenarios/snapshot_edit_coverage.mlc"
SPANS="$ROOT_DIR/scripts/run_editor_perf_spans_visible_only.sh"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_snapshot_edit_coverage_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$EDIT_SCENARIO" ] || fail "missing edit_no_full_flatten.mlc (unexpected drift — §107f)"
[ -f "$SPANS" ] || fail "missing spans-visible harness (unexpected drift — §109f required)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green snapshot-edit harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_snapshot_edit_coverage.sh already present (expected gap until Green)"
fi

# Red: sibling L2 scenario for delete/paste/newline not present yet (or still
# insert-only coverage — Decision pre-cut).
if [ -f "$SIBLING" ]; then
  fail "snapshot_edit_coverage.mlc already present (expected gap until Green)"
fi
if grep -Eqi 'paste|backspace|newline|document_delete' "$EDIT_SCENARIO"; then
  fail "edit_no_full_flatten already covers delete/paste/newline (expected insert-only until Green)"
fi

# Red: paste/cut/backspace/newline still force full snapshot tick (pre-cut).
# Insert already uses tick_snapshot_edit (§107f) — must remain.
if ! grep -q 'frame_layout_tick_snapshot_edit' "$DEMO"; then
  fail "demo_live missing frame_layout_tick_snapshot_edit (unexpected drift — §107f)"
fi

# Count force-1 ticks; Decision audit expects many remain.
force_one_count="$(
  grep -c 'frame_layout_tick_snapshot(frame_layout, document, 1)' "$DEMO" || true
)"
if [ "${force_one_count:-0}" -lt 10 ]; then
  fail "demo_live force-1 snapshot ticks=$force_one_count (<10; unexpected early cut)"
fi

# Backspace / newline / paste paths still adjacent to force-1 (not edit tick).
# Heuristic: lines with editor_app_backspace / insert_newline / edit_paste nearby
# still call tick_snapshot(..., 1) rather than only tick_snapshot_edit.
if ! awk '
  /editor_app_backspace\(/ { want=1 }
  want && /frame_layout_tick_snapshot\(frame_layout, document, 1\)/ { found=1 }
  want && /frame_layout_tick_snapshot_edit\(/ { want=0 }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live backspace path no longer force-1 (expected until Green)"
fi
if ! awk '
  /editor_app_insert_newline\(/ { want=1 }
  want && /frame_layout_tick_snapshot\(frame_layout, document, 1\)/ { found=1 }
  want && /frame_layout_tick_snapshot_edit\(/ { want=0 }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live newline path no longer force-1 (expected until Green)"
fi
if ! awk '
  /edit_paste\(/ { want=1 }
  want && /frame_layout_tick_snapshot\(frame_layout, document, 1\)/ { found=1 }
  want && /frame_layout_tick_snapshot_edit\(/ { want=0 }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  fail "demo_live paste path no longer force-1 (expected until Green)"
fi

# Red: no post-cut Green measured / coverage table yet.
if grep -A40 '### Snapshot.edit Green\|### §109g Green\|### Snapshot-edit Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'flatten_count|tick_snapshot_edit.*paste|force-1.*0|N≥8'; then
  fail "TRACK already has snapshot-edit post-cut measured notes (expected gap until Green)"
fi

fail "no editor perf snapshot-edit coverage harness (Red expected)"
