#!/usr/bin/env bash
# TRACK_EDITOR_CONTENT_SCROLLBAR — scenario content_scrollbar_thumb_on_hover (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/content_scrollbar_thumb_on_hover.mlc"
OUT_DIR="${UX_CONTENT_SCROLLBAR_THUMB_OUT:-$ROOT_DIR/tmp/ux_content_scrollbar_thumb}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux content_scrollbar_thumb_on_hover] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux content_scrollbar_thumb_on_hover] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok content_scrollbar_thumb_on_hover'; then
  echo "[ux content_scrollbar_thumb_on_hover] FAIL: missing ux_ok content_scrollbar_thumb_on_hover" >&2
  exit 1
fi
echo "[ux content_scrollbar_thumb_on_hover] ok" >&2
