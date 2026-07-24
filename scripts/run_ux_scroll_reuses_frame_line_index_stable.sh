#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX — L2 + demo wire for frame LineIndex reuse.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/scroll_reuses_frame_line_index.mlc"
OUT_DIR="${UX_SCROLL_REUSES_FRAME_LINE_INDEX_OUT:-$ROOT_DIR/tmp/ux_scroll_reuses_frame_line_index}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
SCROLL="$ROOT_DIR/misc/editor/ux/scroll.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -q 'line_index_from_document' "$SCROLL"; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: scroll.mlc still calls line_index_from_document" >&2
  exit 1
fi
if ! grep -q 'line_index: LineIndex' "$SCROLL"; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: scroll helpers missing LineIndex param" >&2
  exit 1
fi
if ! grep -q 'editor_app_wheel_scroll(app, metrics, scroll_ticks, line_index)' "$DEMO"; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: demo wheel not passing line_index" >&2
  exit 1
fi
if ! grep -q 'editor_app_ensure_caret_visible(app, metrics, line_index)' "$DEMO"; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: demo ensure_caret not passing line_index" >&2
  exit 1
fi
if ! grep -A2 'editor_app_drag_text_autoscroll(' "$DEMO" | grep -q 'line_index'; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: demo drag not passing line_index" >&2
  exit 1
fi

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
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok scroll_reuses_frame_line_index'; then
  echo "[ux scroll_reuses_frame_line_index_stable] FAIL: missing ux_ok scroll_reuses_frame_line_index" >&2
  exit 1
fi
echo "[ux scroll_reuses_frame_line_index_stable] ok" >&2
