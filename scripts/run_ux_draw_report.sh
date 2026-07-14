#!/usr/bin/env bash
# TRACK_UX_HEADLESS STEP=11 — L2 draw report smoke (MLC_GLFW_VISIBLE=0).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/draw_report_headless.mlc"
OUT_DIR="${UX_DRAW_OUT:-$ROOT_DIR/tmp/ux_draw_report}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux draw] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-0}"

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
  echo "[ux draw] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok draw_report_headless'; then
  echo "[ux draw] FAIL: missing ux_ok draw_report_headless" >&2
  exit 1
fi
echo "[ux draw] ok MLC_GLFW_VISIBLE=$MLC_GLFW_VISIBLE" >&2
