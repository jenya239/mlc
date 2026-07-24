#!/usr/bin/env bash
# TRACK_EDITOR_SCROLLBAR_THUMB_DRAG — scenario scrollbar_thumb_drag_stable.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/scrollbar_thumb_drag_stable.mlc"
OUT_DIR="${UX_SCROLLBAR_THUMB_DRAG_STABLE_OUT:-$ROOT_DIR/tmp/ux_scrollbar_thumb_drag_stable}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux scrollbar_thumb_drag_stable] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux scrollbar_thumb_drag_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok scrollbar_thumb_drag_stable'; then
  echo "[ux scrollbar_thumb_drag_stable] FAIL: missing ux_ok scrollbar_thumb_drag_stable" >&2
  exit 1
fi
echo "[ux scrollbar_thumb_drag_stable] ok" >&2
