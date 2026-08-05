#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110d — Red: no batch-stream harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_batch_stream.sh + solid upload counters / orphan-or-dual-VBO.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_batch_stream.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
TERMINAL_GRID="$ROOT_DIR/misc/editor/terminal/terminal_grid_render.mlc"
SOLID="$ROOT_DIR/misc/gui/solid_renderer.mlc"
PAINT_LIST="$ROOT_DIR/misc/editor/ux/paint_list.mlc"
EDITOR_DIR="$ROOT_DIR/misc/editor"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
PAINT_LIST_HARNESS="$ROOT_DIR/scripts/run_editor_paint_list.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"

fail() {
  echo "[editor_batch_stream_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$TERMINAL_GRID" ] || fail "missing terminal_grid_render.mlc (unexpected drift)"
[ -f "$SOLID" ] || fail "missing solid_renderer.mlc (unexpected drift)"
[ -f "$PAINT_LIST" ] || fail "missing paint_list.mlc (unexpected drift — §110c required)"
[ -d "$EDITOR_DIR" ] || fail "missing misc/editor (unexpected drift)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$PAINT_LIST_HARNESS" ] || fail "missing paint-list harness (unexpected drift — §110c required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green batch-stream harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_batch_stream.sh already present (expected gap until Green)"
fi

# Red: no solid_upload_bytes / bytes_uploaded counters yet.
if grep -R --include='*.mlc' -n -E 'solid_upload_bytes|bytes_uploaded' \
  "$EDITOR_DIR" "$SOLID" >/dev/null 2>&1; then
  fail "sources already define solid_upload_bytes/bytes_uploaded (expected until Green)"
fi

# Red: terminal still issues solid_renderer_rect outside submit (Decision residual).
if ! grep -q 'solid_renderer_rect(' "$TERMINAL_GRID"; then
  fail "terminal_grid_render already has zero solid_renderer_rect (expected gap until Green)"
fi
if ! grep -q 'terminal_grid_draw_cached_backgrounds' "$DEMO"; then
  fail "demo_live no longer calls terminal_grid_draw_cached_backgrounds (expected gap until Green)"
fi

# Red: solid flush still single realloc BufferData only (no orphan / dual-VBO).
if grep -E 'buffer_data_orphan|orphan_buffer|dual_vbo|vertex_buffer_alt|glBufferData\(.*NULL' \
  "$SOLID" >/dev/null 2>&1; then
  fail "solid_renderer already has orphan/dual-VBO markers (expected until Green)"
fi

# Red: ux_gate still does not wire batch-stream harness.
if grep -q 'run_editor_batch_stream\|solid_upload_bytes\|bytes_uploaded' "$UX_GATE"; then
  fail "run_ux_gate already wires batch stream (Decision: not folded)"
fi

# Red: no §110d Green measured paste yet.
if grep -A40 '### §110d Green measured\|Green measured (§110d)\|### Green measured (§110d)' "$TRACK" 2>/dev/null | \
  grep -Eq 'solid_upload_bytes=|solid_flush_count=|draw_calls='; then
  fail "TRACK already has batch-stream post-cut measured numbers (expected gap until Green)"
fi

fail "no editor batch stream harness (Red expected)"
