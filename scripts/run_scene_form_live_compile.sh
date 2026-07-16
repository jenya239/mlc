#!/usr/bin/env bash
# TRACK_SCENE_FORM_LIVE STEP=1 — compile/link gate (no interactive exec).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/scene_form_live.mlc"
OUT_DIR="${SCENE_FORM_LIVE_OUT:-$ROOT_DIR/tmp/scene_form_live_compile}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[scene form live compile] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[scene form live compile] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[scene form live compile] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[scene form live compile] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi
echo "scene_form_live_compile_ok"
echo "[scene form live compile] ok" >&2
