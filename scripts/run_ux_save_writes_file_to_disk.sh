#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107a — Ctrl+S writes the open file to disk.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/save_writes_file_to_disk.mlc"
OUT_DIR="${UX_SAVE_WRITES_FILE_TO_DISK_OUT:-$ROOT_DIR/tmp/ux_save_writes_file_to_disk}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux save_writes_file_to_disk] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR" "$ROOT_DIR/.tmp/ux_save_writes_file_to_disk"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux save_writes_file_to_disk] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok save_writes_file_to_disk'; then
  echo "[ux save_writes_file_to_disk] FAIL: missing ux_ok save_writes_file_to_disk" >&2
  exit 1
fi
echo "[ux save_writes_file_to_disk] ok" >&2
