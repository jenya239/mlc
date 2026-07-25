#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b frame_input STEP=2 — poll owned by frame_input.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
INPUT_MOD="$ROOT_DIR/misc/editor/app/frame_input.mlc"

if [ ! -f "$INPUT_MOD" ]; then
  echo "[editor frame_input_stable] FAIL: missing $INPUT_MOD" >&2
  exit 1
fi
if ! grep -q 'export fn frame_input_poll' "$INPUT_MOD"; then
  echo "[editor frame_input_stable] FAIL: missing frame_input_poll" >&2
  exit 1
fi
if ! grep -q 'export type EditorFrameInput' "$INPUT_MOD"; then
  echo "[editor frame_input_stable] FAIL: missing EditorFrameInput" >&2
  exit 1
fi

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_input_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if ! grep -q 'frame_input_poll(' "$DEMO"; then
  echo "[editor frame_input_stable] FAIL: demo_live missing frame_input_poll(" >&2
  exit 1
fi

poll_count="$(grep -c 'gui_input_poll(' "$DEMO" || true)"
if [ "$poll_count" -ne 0 ]; then
  echo "[editor frame_input_stable] FAIL: demo_live still has gui_input_poll( (count=$poll_count)" >&2
  exit 1
fi
if grep -q 'glfw_gl_take_scroll_y(' "$DEMO"; then
  echo "[editor frame_input_stable] FAIL: demo_live still has glfw_gl_take_scroll_y(" >&2
  exit 1
fi
if grep -q 'glfw_gl_take_binding_key(' "$DEMO"; then
  echo "[editor frame_input_stable] FAIL: demo_live still has glfw_gl_take_binding_key(" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok frame_input"
echo "[editor frame_input_stable] ok" >&2
