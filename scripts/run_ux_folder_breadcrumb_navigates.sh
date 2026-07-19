#!/usr/bin/env bash
# TRACK_EDITOR_UX_L1_BACKFILL — scenario folder_breadcrumb_navigates (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/folder_breadcrumb_navigates.mlc"
OUT_DIR="${UX_FOLDER_BREADCRUMB_NAVIGATES_OUT:-$ROOT_DIR/tmp/ux_folder_breadcrumb_navigates}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux folder_breadcrumb_navigates] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux folder_breadcrumb_navigates] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok folder_breadcrumb_navigates'; then
  echo "[ux folder_breadcrumb_navigates] FAIL: missing ux_ok folder_breadcrumb_navigates" >&2
  exit 1
fi
echo "[ux folder_breadcrumb_navigates] ok" >&2
