#!/usr/bin/env bash
# TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX STEP=1 — selection helpers re-stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SELECTION="$ROOT_DIR/misc/editor/ux/selection_apply.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_selection_reuses_frame_line_index_stable.sh"

if [ ! -f "$SELECTION" ]; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: missing $SELECTION" >&2
  exit 1
fi

# Gap: three selection helpers still call line_index_from_document (re-flatten).
count="$(grep -c 'line_index_from_document(active.buffer.document)' "$SELECTION" || true)"
if [ "$count" -lt 3 ]; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: expected ≥3 line_index_from_document calls in selection_apply.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_click_text' "$SELECTION"; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: missing editor_ux_click_text" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_drag_text' "$SELECTION"; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: missing editor_ux_drag_text" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_selection_draw_report' "$SELECTION"; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: missing editor_ux_selection_draw_report" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok selection_reuses_frame_line_index' "$STABLE"; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux selection_reuses_frame_line_index_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok selection_reuses_frame_line_index_red"
echo "[ux selection_reuses_frame_line_index_red] ok" >&2
