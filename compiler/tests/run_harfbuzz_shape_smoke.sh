#!/usr/bin/env bash
# HarfBuzz TextShaper smoke (TRACK_TEXT_RENDERING STEP=3).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${HARFBUZZ_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/harfbuzz_shape_smoke.mlc}"
OUT_DIR="${HARFBUZZ_SMOKE_OUT:-$ROOT_DIR/tmp/harfbuzz_shape_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists freetype2 || ! pkg-config --exists harfbuzz; then
  echo "[harfbuzz shape smoke] FAIL: freetype2/harfbuzz not found (pkg-config)" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[harfbuzz shape smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[harfbuzz shape smoke] FAIL missing: $ENTRY" >&2
  exit 1
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf ]; then
  echo "[harfbuzz shape smoke] FAIL: DejaVuSans.ttf not found" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$OUT_DIR"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[harfbuzz shape smoke] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[harfbuzz shape smoke] ok" >&2
