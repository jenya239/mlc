#!/usr/bin/env bash
# TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX — L2 + callers for frame LineIndex reuse.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/probe_reuses_frame_line_index.mlc"
OUT_DIR="${UX_PROBE_REUSES_FRAME_LINE_INDEX_OUT:-$ROOT_DIR/tmp/ux_probe_reuses_frame_line_index}"
BIN_OUT="$OUT_DIR/bin"
PROBE="$ROOT_DIR/misc/editor/ux/probe.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux probe_reuses_frame_line_index_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -q 'line_index_from_document' "$PROBE"; then
  echo "[ux probe_reuses_frame_line_index_stable] FAIL: probe still calls line_index_from_document" >&2
  exit 1
fi
if ! grep -q 'line_index: LineIndex' "$PROBE"; then
  echo "[ux probe_reuses_frame_line_index_stable] FAIL: editor_ux_snapshot missing LineIndex param" >&2
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
  echo "[ux probe_reuses_frame_line_index_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok probe_reuses_frame_line_index'; then
  echo "[ux probe_reuses_frame_line_index_stable] FAIL: missing ux_ok probe_reuses_frame_line_index" >&2
  exit 1
fi
echo "[ux probe_reuses_frame_line_index_stable] ok" >&2
