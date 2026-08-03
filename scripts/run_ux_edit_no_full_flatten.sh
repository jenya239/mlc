#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107f — scenario edit_no_full_flatten (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/edit_no_full_flatten.mlc"
OUT_DIR="${UX_EDIT_NO_FULL_FLATTEN_OUT:-$ROOT_DIR/tmp/ux_edit_no_full_flatten}"
BIN_OUT="$OUT_DIR/bin"
LINE_INDEX="$ROOT_DIR/misc/editor/document/line_index.mlc"
FRAME_SNAPSHOT="$ROOT_DIR/misc/editor/document/frame_snapshot.mlc"
FRAME_CACHE="$ROOT_DIR/misc/editor/document/frame_snapshot_cache.mlc"
FRAME_LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux edit_no_full_flatten] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! grep -Eq 'export fn line_index_apply_edit\(' "$LINE_INDEX"; then
  echo "[ux edit_no_full_flatten] FAIL: line_index_apply_edit missing" >&2
  exit 1
fi
if ! grep -q 'document_frame_snapshot_apply_edit' "$FRAME_SNAPSHOT"; then
  echo "[ux edit_no_full_flatten] FAIL: document_frame_snapshot_apply_edit missing" >&2
  exit 1
fi
if ! grep -q 'frame_snapshot_cache_apply_edit' "$FRAME_CACHE"; then
  echo "[ux edit_no_full_flatten] FAIL: frame_snapshot_cache_apply_edit missing" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_snapshot_edit' "$FRAME_LAYOUT"; then
  echo "[ux edit_no_full_flatten] FAIL: frame_layout_tick_snapshot_edit missing" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_snapshot_edit' "$DEMO"; then
  echo "[ux edit_no_full_flatten] FAIL: demo_live not wired to snapshot_edit" >&2
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
  echo "[ux edit_no_full_flatten] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok edit_no_full_flatten'; then
  echo "[ux edit_no_full_flatten] FAIL: missing ux_ok edit_no_full_flatten" >&2
  exit 1
fi
echo "[ux edit_no_full_flatten] ok" >&2
