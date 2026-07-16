#!/usr/bin/env bash
# TRACK_STDLIB_IO_FS STEP=6 — demo_live compile gate (real/mock tree).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/demo_live.mlc"
OUT_DIR="${EDITOR_DEMO_LIVE_FS_OUT:-$ROOT_DIR/tmp/editor_demo_live_fs_compile}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[demo_live fs compile] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[demo_live fs compile] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[demo_live fs compile] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-0}"
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
if [ ! -x "$BIN_OUT" ]; then
  echo "[demo_live fs compile] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi
echo "demo_live_fs_compile_ok"
echo "[demo_live fs compile] ok" >&2
