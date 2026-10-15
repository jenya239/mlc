#!/usr/bin/env bash
# File clipboard offers text/uri-list and x-special/gnome-copied-files on X11.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/file_clipboard_x11_smoke.mlc"
OUT_DIR="${TEXTUI_FILE_CLIPBOARD_OUT:-$ROOT_DIR/tmp/textui_file_clipboard_x11}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[textui file clipboard] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f /usr/include/X11/Xlib.h ]; then
  echo "[textui file clipboard] SKIP: X11 headers missing" >&2
  exit 0
fi
if [ -z "${DISPLAY:-}" ]; then
  echo "[textui file clipboard] SKIP: DISPLAY is unset" >&2
  exit 0
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
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[textui file clipboard] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui file clipboard] ok" >&2
