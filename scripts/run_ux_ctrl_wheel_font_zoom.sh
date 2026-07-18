#!/usr/bin/env bash
# TRACK_EDITOR_FONT_ZOOM STEP=1 — scenario ctrl_wheel_font_zoom (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/ctrl_wheel_font_zoom.mlc"
OUT_DIR="${UX_FONT_ZOOM_OUT:-$ROOT_DIR/tmp/ux_ctrl_wheel_font_zoom}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux font_zoom] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux font_zoom] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok ctrl_wheel_font_zoom'; then
  echo "[ux font_zoom] FAIL: missing ux_ok ctrl_wheel_font_zoom" >&2
  exit 1
fi
echo "[ux font_zoom] ok" >&2
