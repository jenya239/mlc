#!/usr/bin/env bash
# Interactive mlc-editor preview (no auto-close). Esc / window close to quit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/demo_live.mlc"
OUT_DIR="${EDITOR_LIVE_OUT:-$ROOT_DIR/tmp/editor_live}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[editor live] FAIL: glfw3 not found" >&2
  exit 1
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[editor live] FAIL: DejaVuSansMono.ttf not found" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[editor live] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE=1

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
exec "$BIN_OUT"
