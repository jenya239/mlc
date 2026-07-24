#!/usr/bin/env bash
# TRACK_EDITOR_MULTI_CARET_DRAW — scenario multi_caret_draw_stable.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/multi_caret_draw_stable.mlc"
OUT_DIR="${UX_MULTI_CARET_DRAW_STABLE_OUT:-$ROOT_DIR/tmp/ux_multi_caret_draw_stable}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ] &&
   [ ! -f /usr/share/fonts/TTF/DejaVuSansMono.ttf ]; then
  echo "[ux multi_caret_draw_stable] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi

if [ ! -x "$MLCC" ]; then
  echo "[ux multi_caret_draw_stable] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux multi_caret_draw_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok multi_caret_draw_stable'; then
  echo "[ux multi_caret_draw_stable] FAIL: missing ux_ok multi_caret_draw_stable" >&2
  exit 1
fi
echo "[ux multi_caret_draw_stable] ok" >&2
