#!/usr/bin/env bash
# TRACK_EDITOR_TREE_EXPAND_COLLAPSE STEP=1 — confirm L1 stub is still red.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/tree_row_click_toggles_expand.mlc"
OUT_DIR="$ROOT_DIR/tmp/ux_tree_expand_red"
BIN_OUT="$OUT_DIR/bin"
FAIL_TOKEN="ux_fail tree_row_click_toggles_expand stub"

if [ ! -x "$MLCC" ]; then
  echo "[ux tree_expand_red] FAIL: mlcc not found at $MLCC" >&2
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
if [ "$status" -eq 0 ]; then
  echo "[ux tree_expand_red] FAIL: exit 0 (expected red)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q "$FAIL_TOKEN"; then
  echo "[ux tree_expand_red] FAIL: missing $FAIL_TOKEN" >&2
  exit 1
fi

echo "ux_ok tree_expand_red"
echo "[ux tree_expand_red] ok" >&2
