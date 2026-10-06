#!/usr/bin/env bash
# Textui chat: compile and run every misc/textui/test/chat*.mlc. No GLFW.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"

if [ ! -x "$MLCC" ]; then
  echo "[textui chat] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

shopt -s nullglob
entries=("$ROOT_DIR"/misc/textui/test/chat*.mlc)
if [ "${#entries[@]}" -eq 0 ]; then
  echo "[textui chat] FAIL: no chat tests" >&2
  exit 1
fi

for entry in "${entries[@]}"; do
  name="$(basename "$entry" .mlc)"
  out_dir="$ROOT_DIR/tmp/textui_chat_smoke/$name"
  binary="$out_dir/bin"
  rm -rf "$out_dir"
  mkdir -p "$out_dir"
  "$MLCC" -o "$out_dir" "$entry"
  "$COMPILER_DIR/build_bin.sh" "$out_dir" "$binary"
  set +e
  "$binary"
  status=$?
  set -e
  if [ "$status" -ne 0 ]; then
    echo "[textui chat] FAIL $name exit=$status" >&2
    exit 1
  fi
  echo "[textui chat] $name ok" >&2
done
