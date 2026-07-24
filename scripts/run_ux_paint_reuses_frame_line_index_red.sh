#!/usr/bin/env bash
# TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX STEP=1 — paint overlays re-stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CURRENT_LINE_HL="$ROOT_DIR/misc/editor/ux/current_line_hl.mlc"
TRAILING_WS="$ROOT_DIR/misc/editor/ux/trailing_ws.mlc"
STATUS_BAR="$ROOT_DIR/misc/editor/ui/status_bar.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_paint_reuses_frame_line_index_stable.sh"

for path in "$CURRENT_LINE_HL" "$TRAILING_WS" "$STATUS_BAR"; do
  if [ ! -f "$path" ]; then
    echo "[ux paint_reuses_frame_line_index_red] FAIL: missing $path" >&2
    exit 1
  fi
done

# Gap: paint helpers still call line_index_from_document (re-flatten).
if ! grep -q 'line_index_from_document' "$CURRENT_LINE_HL"; then
  echo "[ux paint_reuses_frame_line_index_red] FAIL: current_line_hl missing line_index_from_document (expected gap)" >&2
  exit 1
fi
if ! grep -q 'line_index_from_document' "$TRAILING_WS"; then
  echo "[ux paint_reuses_frame_line_index_red] FAIL: trailing_ws missing line_index_from_document (expected gap)" >&2
  exit 1
fi
if ! grep -q 'line_index_from_document' "$STATUS_BAR"; then
  echo "[ux paint_reuses_frame_line_index_red] FAIL: status_bar missing line_index_from_document (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux paint_reuses_frame_line_index_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok paint_reuses_frame_line_index' "$STABLE"; then
  echo "[ux paint_reuses_frame_line_index_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux paint_reuses_frame_line_index_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok paint_reuses_frame_line_index_red"
echo "[ux paint_reuses_frame_line_index_red] ok" >&2
