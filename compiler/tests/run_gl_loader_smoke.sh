#!/usr/bin/env bash
# EGL headless glGenBuffers via eglGetProcAddress (TRACK_TEXT_RENDERING STEP=5).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GL_LOADER_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/gl_loader_smoke.mlc}"
OUT_DIR="${GL_LOADER_SMOKE_OUT:-$ROOT_DIR/tmp/gl_loader_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists egl || ! pkg-config --exists glesv2; then
  echo "[gl loader smoke] FAIL: egl/glesv2 not found (pkg-config)" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[gl loader smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[gl loader smoke] FAIL missing: $ENTRY" >&2
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
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[gl loader smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[gl loader smoke] ok" >&2
