#!/usr/bin/env bash
# TRACK_STDLIB_IO_FS STEP=5 — FileTree expand-from-disk unit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${EDITOR_FILE_TREE_FS_ENTRY:-$ROOT_DIR/misc/editor/tests/file_tree_fs_unit.mlc}"
OUT_DIR="${EDITOR_FILE_TREE_FS_OUT:-$ROOT_DIR/tmp/editor_file_tree_fs}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[editor file_tree_fs] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
rm -rf "$ROOT_DIR/.tmp/editor_file_tree_fs_fixture"
mkdir -p "$OUT_DIR"
mkdir -p "$ROOT_DIR/.tmp"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[editor file_tree_fs] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q '\[mlc-editor\] file_tree_fs_unit ok'; then
  echo "[editor file_tree_fs] FAIL: missing file_tree_fs_unit ok" >&2
  exit 1
fi
echo "[editor file_tree_fs] ok" >&2
