#!/usr/bin/env bash
# GLFW window empty clear/poll/swap loop (TRACK_TEXT_RENDERING_NATIVE STEP=2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GLFW_WINDOW_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/glfw_window_smoke.mlc}"
OUT_DIR="${GLFW_WINDOW_SMOKE_OUT:-$ROOT_DIR/tmp/glfw_window_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[glfw window smoke] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[glfw window smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[glfw window smoke] FAIL missing: $ENTRY" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
# Automated: hidden window (override with MLC_GLFW_VISIBLE=1 for interactive).
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[glfw window smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[glfw window smoke] ok" >&2
