#!/usr/bin/env bash
# TRACK_EDITOR_TAB_STRIP_CLASSIC_LAYOUT — scenario tab_strip_long_title_ellipsis (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/tab_strip_long_title_ellipsis.mlc"
OUT_DIR="${UX_TAB_STRIP_LONG_TITLE_ELLIPSIS_OUT:-$ROOT_DIR/tmp/ux_tab_strip_long_title_ellipsis}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux tab_strip_long_title_ellipsis] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux tab_strip_long_title_ellipsis] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok tab_strip_long_title_ellipsis'; then
  echo "[ux tab_strip_long_title_ellipsis] FAIL: missing ux_ok tab_strip_long_title_ellipsis" >&2
  exit 1
fi
echo "[ux tab_strip_long_title_ellipsis] ok" >&2
