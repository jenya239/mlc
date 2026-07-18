#!/usr/bin/env bash
# TRACK_EDITOR_TRAILING_WS_VIZ STEP=1 — scenario trailing_ws_visualized (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/trailing_ws_visualized.mlc"
OUT_DIR="${UX_TRAILING_WS_VIZ_OUT:-$ROOT_DIR/tmp/ux_trailing_ws_visualized}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux trailing_ws_visualized] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux trailing_ws_visualized] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok trailing_ws_visualized'; then
  echo "[ux trailing_ws_visualized] FAIL: missing ux_ok trailing_ws_visualized" >&2
  exit 1
fi
echo "[ux trailing_ws_visualized] ok" >&2
