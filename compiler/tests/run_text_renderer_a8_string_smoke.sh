#!/usr/bin/env bash
# Shaped-string A8 TextRenderer + golden fixtures (STEP=6.2 + TEXT_GL STEP=9).
# Runs "Привет" and mixed "Hxpjy Agq" (MAE≤8 vs CPU; golden RGBA lock).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXT_RENDERER_STRING_ENTRY:-$ROOT_DIR/misc/examples/text_renderer_a8_string_smoke.mlc}"
OUT_DIR="${TEXT_RENDERER_STRING_OUT:-$ROOT_DIR/tmp/text_renderer_a8_string_smoke}"
BIN_OUT="$OUT_DIR/bin"
GOLDEN="${TEXT_RENDERER_GOLDEN:-$ROOT_DIR/misc/examples/fixtures/text_a8_privet_24.rgba}"

need_pkg() {
  if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists "$1"; then
    echo "[text renderer string] FAIL: pkg-config $1 missing" >&2
    exit 1
  fi
}
need_pkg egl
need_pkg glesv2
need_pkg freetype2
need_pkg harfbuzz

FONT="${TEXT_RENDERER_FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"
if [ ! -f "$FONT" ]; then
  echo "[text renderer string] FAIL missing font: $FONT" >&2
  exit 1
fi
if [ ! -x "$MLCC" ]; then
  echo "[text renderer string] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[text renderer string] FAIL missing: $ENTRY" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR" "$(dirname "$GOLDEN")"

# Smoke uses cwd-relative golden path from the .mlc source.
cd "$ROOT_DIR"

if [ ! -f "$GOLDEN" ]; then
  echo "[text renderer string] writing golden $GOLDEN" >&2
  export MLC_WRITE_GOLDEN=1
fi

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[text renderer string] FAIL binary exit=$status" >&2
  exit 1
fi
if [ ! -f "$GOLDEN" ]; then
  echo "[text renderer string] FAIL golden not written: $GOLDEN" >&2
  exit 1
fi
echo "[text renderer string] ok" >&2
