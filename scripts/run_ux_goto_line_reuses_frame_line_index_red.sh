#!/usr/bin/env bash
# TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX STEP=1 — goto helpers re-stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GOTO_LINE="$ROOT_DIR/misc/editor/ux/goto_line.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_goto_line_reuses_frame_line_index_stable.sh"

if [ ! -f "$GOTO_LINE" ]; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: missing $GOTO_LINE" >&2
  exit 1
fi

# Gap: two goto helpers still call line_index_from_document (re-flatten).
count="$(grep -c 'line_index_from_document(active.buffer.document)' "$GOTO_LINE" || true)"
if [ "$count" -lt 2 ]; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: expected ≥2 line_index_from_document calls in goto_line.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_goto_line(' "$GOTO_LINE"; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: missing editor_ux_goto_line" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_goto_line_from_selection(' "$GOTO_LINE"; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: missing editor_ux_goto_line_from_selection" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok goto_line_reuses_frame_line_index' "$STABLE"; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux goto_line_reuses_frame_line_index_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok goto_line_reuses_frame_line_index_red"
echo "[ux goto_line_reuses_frame_line_index_red] ok" >&2
