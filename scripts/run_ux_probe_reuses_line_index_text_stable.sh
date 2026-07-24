#!/usr/bin/env bash
# TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT — L2: snapshot text from line_index.text.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/probe_reuses_line_index_text.mlc"
OUT_DIR="${UX_PROBE_REUSES_LINE_INDEX_TEXT_OUT:-$ROOT_DIR/tmp/ux_probe_reuses_line_index_text}"
BIN_OUT="$OUT_DIR/bin"
PROBE="$ROOT_DIR/misc/editor/ux/probe.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux probe_reuses_line_index_text_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -q 'document_to_string' "$PROBE"; then
  echo "[ux probe_reuses_line_index_text_stable] FAIL: probe still calls document_to_string" >&2
  exit 1
fi
if ! grep -q 'document_text = line_index.text' "$PROBE"; then
  echo "[ux probe_reuses_line_index_text_stable] FAIL: probe missing line_index.text assignment" >&2
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
  echo "[ux probe_reuses_line_index_text_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok probe_reuses_line_index_text'; then
  echo "[ux probe_reuses_line_index_text_stable] FAIL: missing ux_ok probe_reuses_line_index_text" >&2
  exit 1
fi
echo "[ux probe_reuses_line_index_text_stable] ok" >&2
