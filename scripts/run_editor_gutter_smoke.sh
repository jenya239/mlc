#!/usr/bin/env bash
# TRACK_EDITOR_MVP STEP=12 — gutter line numbers visual smoke (GLFW).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${EDITOR_GUTTER_SMOKE_ENTRY:-$ROOT_DIR/misc/editor/tests/gutter_smoke.mlc}"
OUT_DIR="${EDITOR_GUTTER_SMOKE_OUT:-$ROOT_DIR/tmp/editor_gutter_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[editor gutter smoke] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[editor gutter smoke] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[editor gutter smoke] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[editor gutter smoke] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] open'; then
  echo "[editor gutter smoke] FAIL: missing [mlc-editor] open" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -qE '\[mlc-editor\] gutter ok glyphs=[1-9][0-9]*'; then
  echo "[editor gutter smoke] FAIL: missing gutter ok glyphs=N" >&2
  exit 1
fi
echo "[editor gutter smoke] ok" >&2
