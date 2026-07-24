#!/usr/bin/env bash
# TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX STEP=1 — overflow helpers re-stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OVERFLOW="$ROOT_DIR/misc/editor/ux/overflow.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_overflow_reuses_frame_line_index_stable.sh"

if [ ! -f "$OVERFLOW" ]; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: missing $OVERFLOW" >&2
  exit 1
fi

# Gap: content_rect_for_state still calls line_index_from_document (re-flatten).
count="$(grep -c 'line_index_from_document(active.buffer.document)' "$OVERFLOW" || true)"
if [ "$count" -lt 1 ]; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: expected ≥1 line_index_from_document call in overflow.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_collect_overflows(' "$OVERFLOW"; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: missing editor_ux_collect_overflows" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_refresh_overflows(' "$OVERFLOW"; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: missing editor_ux_refresh_overflows" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok overflow_reuses_frame_line_index' "$STABLE"; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux overflow_reuses_frame_line_index_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok overflow_reuses_frame_line_index_red"
echo "[ux overflow_reuses_frame_line_index_red] ok" >&2
