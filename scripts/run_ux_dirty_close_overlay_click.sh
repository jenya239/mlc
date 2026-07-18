#!/usr/bin/env bash
# TRACK_EDITOR_DIRTY_CLOSE_L1 STEP=1 — scenario dirty_close_overlay_click (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/dirty_close_overlay_click.mlc"
OUT_DIR="${UX_DIRTY_CLOSE_OVERLAY_CLICK_OUT:-$ROOT_DIR/tmp/ux_dirty_close_overlay_click}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux dirty_close_overlay_click] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux dirty_close_overlay_click] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok dirty_close_overlay_click'; then
  echo "[ux dirty_close_overlay_click] FAIL: missing ux_ok dirty_close_overlay_click" >&2
  exit 1
fi
echo "[ux dirty_close_overlay_click] ok" >&2
