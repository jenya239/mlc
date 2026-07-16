#!/usr/bin/env bash
# TRACK_EDITOR_FONT_CONFIG STEP=1 — font path resolver unit (no GLFW).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${EDITOR_FONT_CONFIG_ENTRY:-$ROOT_DIR/misc/editor/tests/font_config_unit.mlc}"
OUT_DIR="${EDITOR_FONT_CONFIG_OUT:-$ROOT_DIR/tmp/editor_font_config}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor font_config] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"

run_once() {
  local label=$1
  set +e
  output=$("$BIN_OUT" 2>&1)
  status=$?
  set -e
  printf '%s\n' "$output"
  if [ "$status" -ne 0 ]; then
    echo "[editor font_config] FAIL $label exit=$status" >&2
    exit 1
  fi
  if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] font_config_unit ok'; then
    echo "[editor font_config] FAIL $label: missing font_config_unit ok" >&2
    exit 1
  fi
}

unset MLC_EDITOR_FONT || true
run_once default

export MLC_EDITOR_FONT="/tmp/mlc_editor_font_override.ttf"
run_once env_override

echo "[editor font_config] ok" >&2
