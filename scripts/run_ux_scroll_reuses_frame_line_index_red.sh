#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX STEP=1 — scroll helpers re-stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SCROLL="$ROOT_DIR/misc/editor/ux/scroll.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_scroll_reuses_frame_line_index_stable.sh"

if [ ! -f "$SCROLL" ]; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: missing $SCROLL" >&2
  exit 1
fi

# Gap: three scroll helpers still call line_index_from_document (re-flatten).
count="$(grep -c 'line_index_from_document(active.buffer.document)' "$SCROLL" || true)"
if [ "$count" -lt 3 ]; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: expected ≥3 line_index_from_document calls in scroll.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_clamp_scroll_y' "$SCROLL"; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: missing editor_ux_clamp_scroll_y" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_caret_in_viewport' "$SCROLL"; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: missing editor_ux_caret_in_viewport" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_ensure_caret_visible' "$SCROLL"; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: missing editor_ux_ensure_caret_visible" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok scroll_reuses_frame_line_index' "$STABLE"; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux scroll_reuses_frame_line_index_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok scroll_reuses_frame_line_index_red"
echo "[ux scroll_reuses_frame_line_index_red] ok" >&2
