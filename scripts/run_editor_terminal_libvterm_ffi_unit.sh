#!/usr/bin/env bash
# TRACK_EDITOR_TERMINAL §102a — libvterm FFI round-trip (no PTY, no rendering).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/tests/terminal_libvterm_ffi_unit.mlc"
OUT_DIR="${EDITOR_TERMINAL_LIBVTERM_FFI_OUT:-$ROOT_DIR/tmp/editor_terminal_libvterm_ffi}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor terminal_libvterm_ffi] FAIL: mlcc not found at $MLCC" >&2
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
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[editor terminal_libvterm_ffi] FAIL exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] terminal_libvterm_ffi_unit ok'; then
  echo "[editor terminal_libvterm_ffi] FAIL: missing terminal_libvterm_ffi_unit ok" >&2
  exit 1
fi

echo "[editor terminal_libvterm_ffi] ok" >&2
