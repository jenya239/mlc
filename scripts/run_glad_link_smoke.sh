#!/usr/bin/env bash
# Link-check: glad_bindings_smoke + build_bin (links vendored glad gl.c when glfw3).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${GLAD_LINK_SMOKE_ENTRY:-$ROOT_DIR/misc/examples/glad_bindings_smoke.mlc}"
OUT_DIR="${GLAD_LINK_SMOKE_OUT:-$ROOT_DIR/tmp/glad_link_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[glad link smoke] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[glad link smoke] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"

nm --defined-only "$BIN_OUT" > "$OUT_DIR/nm_defined.txt"
if ! grep -q 'gladLoaderLoadGL' "$OUT_DIR/nm_defined.txt"; then
  echo "[glad link smoke] FAIL: gladLoaderLoadGL not in binary" >&2
  exit 1
fi
if ! grep -q 'glad_glClear' "$OUT_DIR/nm_defined.txt"; then
  echo "[glad link smoke] FAIL: glad_glClear not in binary" >&2
  exit 1
fi

echo "[glad link smoke] ok (linked glad desktop)" >&2
