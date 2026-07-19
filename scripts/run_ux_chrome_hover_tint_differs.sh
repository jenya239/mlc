#!/usr/bin/env bash
# TRACK_EDITOR_CHROME_HOVER_STATE — scenario chrome_hover_tint_differs (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/chrome_hover_tint_differs.mlc"
OUT_DIR="${UX_CHROME_HOVER_TINT_DIFFERS_OUT:-$ROOT_DIR/tmp/ux_chrome_hover_tint_differs}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux chrome_hover_tint_differs] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux chrome_hover_tint_differs] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok chrome_hover_tint_differs'; then
  echo "[ux chrome_hover_tint_differs] FAIL: missing ux_ok chrome_hover_tint_differs" >&2
  exit 1
fi
echo "[ux chrome_hover_tint_differs] ok" >&2
