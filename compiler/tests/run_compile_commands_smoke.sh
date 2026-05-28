#!/usr/bin/env bash
# Smoke: mlcc --emit-compile-commands writes compile_commands.json to out dir.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
MLCC="${1:-$ROOT_DIR/compiler/out/mlcc}"
SOURCE="$SCRIPT_DIR/e2e/hello.mlc"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

echo "[compile_commands smoke] mlcc=$MLCC" >&2

if ! "$MLCC" --emit-compile-commands -o "$tmpdir" "$SOURCE" 2>/tmp/compile_commands_smoke_err; then
  echo "[compile_commands smoke] FAIL: mlcc error: $(cat /tmp/compile_commands_smoke_err)" >&2
  exit 1
fi

json="$tmpdir/compile_commands.json"
if [ ! -f "$json" ]; then
  echo "[compile_commands smoke] FAIL: missing $json" >&2
  exit 1
fi

grep -q '"directory"' "$json"
grep -q '"file"' "$json"
grep -q '"arguments"' "$json"
grep -q 'clang++' "$json"
grep -q 'runtime/include' "$json"

echo "[compile_commands smoke] ok" >&2
