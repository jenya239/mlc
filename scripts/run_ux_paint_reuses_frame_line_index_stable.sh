#!/usr/bin/env bash
# TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX — L2 + demo wire for frame LineIndex reuse.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/paint_reuses_frame_line_index.mlc"
OUT_DIR="${UX_PAINT_REUSES_FRAME_LINE_INDEX_OUT:-$ROOT_DIR/tmp/ux_paint_reuses_frame_line_index}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
CURRENT_LINE_HL="$ROOT_DIR/misc/editor/ux/current_line_hl.mlc"
TRAILING_WS="$ROOT_DIR/misc/editor/ux/trailing_ws.mlc"
STATUS_BAR="$ROOT_DIR/misc/editor/ui/status_bar.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux paint_reuses_frame_line_index_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

for path in "$CURRENT_LINE_HL" "$TRAILING_WS" "$STATUS_BAR"; do
  if grep -q 'line_index_from_document' "$path"; then
    echo "[ux paint_reuses_frame_line_index_stable] FAIL: $path still calls line_index_from_document" >&2
    exit 1
  fi
done

if ! grep -q 'editor_ux_current_line_draw_report(' "$DEMO"; then
  echo "[ux paint_reuses_frame_line_index_stable] FAIL: demo missing current_line call" >&2
  exit 1
fi
if ! grep -A2 'editor_ux_current_line_draw_report(' "$DEMO" | grep -q 'line_index'; then
  echo "[ux paint_reuses_frame_line_index_stable] FAIL: demo current_line not passing line_index" >&2
  exit 1
fi
if ! grep -A2 'editor_ux_trailing_ws_draw_report(' "$DEMO" | grep -q 'line_index'; then
  echo "[ux paint_reuses_frame_line_index_stable] FAIL: demo trailing_ws not passing line_index" >&2
  exit 1
fi
if ! grep -q 'active.buffer, selection_caret(selection), line_index' "$DEMO"; then
  echo "[ux paint_reuses_frame_line_index_stable] FAIL: demo status_bar not passing line_index" >&2
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
  echo "[ux paint_reuses_frame_line_index_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok paint_reuses_frame_line_index'; then
  echo "[ux paint_reuses_frame_line_index_stable] FAIL: missing ux_ok paint_reuses_frame_line_index" >&2
  exit 1
fi
echo "[ux paint_reuses_frame_line_index_stable] ok" >&2
