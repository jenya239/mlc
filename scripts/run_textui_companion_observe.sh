#!/usr/bin/env bash
# Companion step 1: observation ring, no GLFW window.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXTUI_COMPANION_OBSERVE_ENTRY:-$ROOT_DIR/misc/textui/test/companion_ring_unit.mlc}"
OUT_DIR="${TEXTUI_COMPANION_OBSERVE_OUT:-$ROOT_DIR/tmp/textui_companion_observe}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[textui companion] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[textui companion] FAIL missing: $ENTRY" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
if [ -d "$ROOT_DIR/tmp/debs/extract/usr/include" ]; then
  export CPLUS_INCLUDE_PATH="$ROOT_DIR/tmp/debs/extract/usr/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[textui companion] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui companion] ok" >&2
