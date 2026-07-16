#!/usr/bin/env bash
# TRACK_GLFW_CONTENT_SCALE STEP=1 — compile/link gate (token check = STEP=2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/glfw_content_scale_smoke.mlc"
OUT_DIR="${GLFW_CONTENT_SCALE_OUT:-$ROOT_DIR/tmp/glfw_content_scale_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[content_scale compile] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[content_scale compile] FAIL: mlcc not found at $MLCC" >&2
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
if [ ! -x "$BIN_OUT" ]; then
  echo "[content_scale compile] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi
echo "content_scale_compile_ok"
echo "[content_scale compile] ok" >&2
