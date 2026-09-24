#!/usr/bin/env bash
# Hover frame cost: full repaint versus the two rows that changed. No visible window.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${TEXTUI_HOVER_BENCH_ENTRY:-$ROOT_DIR/misc/textui/test/hover_frame_bench.mlc}"
OUT_DIR="${TEXTUI_HOVER_BENCH_OUT:-$ROOT_DIR/tmp/textui_hover_bench}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[textui hover] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$ENTRY" ]; then
  echo "[textui hover] FAIL missing: $ENTRY" >&2
  exit 1
fi
if ! grep -q 'glfw_gl_swap_interval(0)' "$ROOT_DIR/misc/textui/app.mlc"; then
  echo "[textui hover] FAIL: gallery must call glfw_gl_swap_interval(0)" >&2
  exit 1
fi
if ! grep -q 'submit_ui_frame' "$ROOT_DIR/misc/textui/app.mlc"; then
  echo "[textui hover] FAIL: gallery hover must go through submit_ui_frame" >&2
  exit 1
fi
ruby -e '
source = File.read(ARGV[0])
body = source[/export fn submit_ui_frame.*?^end/m]
if body.nil?
  warn "[textui hover] FAIL: submit_ui_frame missing"
  exit 1
end
unless body.include?("paint_hover_damage") && body.include?("frame_signature_hover_only")
  warn "[textui hover] FAIL: hover diffs must go through paint_hover_damage"
  exit 1
end
' "$ROOT_DIR/misc/textui/hover_damage.mlc"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
unset MLC_GLFW_VISIBLE

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
"$BIN_OUT"
status=$?
set -e
if [ "$status" -ne 0 ]; then
  echo "[textui hover] FAIL binary exit=$status" >&2
  exit 1
fi
echo "[textui hover] ok" >&2
