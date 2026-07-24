#!/usr/bin/env bash
# TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX — L2 + callers for frame LineIndex reuse.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/goto_line_reuses_frame_line_index.mlc"
OUT_DIR="${UX_GOTO_LINE_REUSES_FRAME_LINE_INDEX_OUT:-$ROOT_DIR/tmp/ux_goto_line_reuses_frame_line_index}"
BIN_OUT="$OUT_DIR/bin"
GOTO_LINE="$ROOT_DIR/misc/editor/ux/goto_line.mlc"
STATE="$ROOT_DIR/misc/editor/app/state.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -q 'line_index_from_document' "$GOTO_LINE"; then
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL: goto_line still calls line_index_from_document" >&2
  exit 1
fi
if ! grep -q 'line_index: LineIndex' "$GOTO_LINE"; then
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL: goto helpers missing LineIndex param" >&2
  exit 1
fi
if ! grep -q 'editor_ux_goto_line_from_selection(app.ux, metrics, line_index)' "$STATE"; then
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL: app/state not passing line_index" >&2
  exit 1
fi
if ! grep -A2 'editor_app_goto_line_from_selection(' "$DEMO" | grep -q 'line_index'; then
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL: demo goto not passing line_index" >&2
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
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok goto_line_reuses_frame_line_index'; then
  echo "[ux goto_line_reuses_frame_line_index_stable] FAIL: missing ux_ok goto_line_reuses_frame_line_index" >&2
  exit 1
fi
echo "[ux goto_line_reuses_frame_line_index_stable] ok" >&2
