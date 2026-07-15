#!/usr/bin/env bash
# TRACK_EDITOR_LIVE_POLISH STEP=1 — gl_scissor smoke (GLFW, MLC_GLFW_VISIBLE=0).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/gl_scissor_smoke.mlc"
OUT_DIR="${GL_SCISSOR_OUT:-$ROOT_DIR/tmp/gl_scissor_smoke}"
BIN_OUT="$OUT_DIR/bin"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[gl scissor] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[gl scissor] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[gl scissor] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'scissor_ok'; then
  echo "[gl scissor] FAIL: missing scissor_ok" >&2
  exit 1
fi
echo "[gl scissor] ok" >&2
