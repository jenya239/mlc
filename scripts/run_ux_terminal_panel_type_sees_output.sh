#!/usr/bin/env bash
# TRACK_EDITOR_TERMINAL §102f — open terminal tab, type, see output (UX gate).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/terminal_panel_type_sees_output.mlc"
OUT_DIR="${UX_TERMINAL_PANEL_TYPE_SEES_OUTPUT_OUT:-$ROOT_DIR/tmp/ux_terminal_panel_type_sees_output}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[ux terminal_panel_type_sees_output] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[ux terminal_panel_type_sees_output] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux terminal_panel_type_sees_output] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok terminal_panel_type_sees_output'; then
  echo "[ux terminal_panel_type_sees_output] FAIL: missing ux_ok" >&2
  exit 1
fi
echo "[ux terminal_panel_type_sees_output] ok" >&2
