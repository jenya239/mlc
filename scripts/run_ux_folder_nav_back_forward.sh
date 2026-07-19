#!/usr/bin/env bash
# TRACK_EDITOR_UX_L1_BACKFILL — scenario folder_nav_back_forward (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/folder_nav_back_forward.mlc"
OUT_DIR="${UX_FOLDER_NAV_BACK_FORWARD_OUT:-$ROOT_DIR/tmp/ux_folder_nav_back_forward}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux folder_nav_back_forward] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux folder_nav_back_forward] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok folder_nav_back_forward'; then
  echo "[ux folder_nav_back_forward] FAIL: missing ux_ok folder_nav_back_forward" >&2
  exit 1
fi
echo "[ux folder_nav_back_forward] ok" >&2
