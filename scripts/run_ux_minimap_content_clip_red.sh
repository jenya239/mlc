#!/usr/bin/env bash
# TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP STEP=1 — confirm L2 stub is still red.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/minimap_content_clip_stable.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
OUT_DIR="$ROOT_DIR/tmp/ux_minimap_content_clip_red"
BIN_OUT="$OUT_DIR/bin"
FAIL_TOKEN="ux_fail minimap_content_clip_stable stub"

if [ ! -x "$MLCC" ]; then
  echo "[ux minimap_content_clip_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! grep -q 'editor_gl_scissor_enable_rect(height, editor_rect)' "$DEMO"; then
  echo "[ux minimap_content_clip_red] FAIL: demo_live missing editor_rect content scissor" >&2
  exit 1
fi
if grep -q 'editor_ux_content_clip_rect' "$DEMO"; then
  echo "[ux minimap_content_clip_red] FAIL: demo_live already wires content clip (expected gap)" >&2
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
if [ "$status" -eq 0 ]; then
  echo "[ux minimap_content_clip_red] FAIL: exit 0 (expected red)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q "$FAIL_TOKEN"; then
  echo "[ux minimap_content_clip_red] FAIL: missing $FAIL_TOKEN" >&2
  exit 1
fi

echo "ux_ok minimap_content_clip_red"
echo "[ux minimap_content_clip_red] ok" >&2
