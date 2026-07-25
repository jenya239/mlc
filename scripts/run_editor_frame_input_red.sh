#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b frame_input STEP=1 — poll still inline in demo_live.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
INPUT_MOD="$ROOT_DIR/misc/editor/app/frame_input.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_frame_input_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_input_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: no frame_input module yet.
if [ -f "$INPUT_MOD" ]; then
  echo "[editor frame_input_red] FAIL: frame_input.mlc already exists (expected gap)" >&2
  exit 1
fi
if grep -q 'frame_input_poll' "$DEMO"; then
  echo "[editor frame_input_red] FAIL: demo_live already uses frame_input_poll (expected gap)" >&2
  exit 1
fi

# Gap: bare gui_input_poll in demo_live.
poll_count="$(grep -c 'gui_input_poll(' "$DEMO" || true)"
if [ "$poll_count" -lt 1 ]; then
  echo "[editor frame_input_red] FAIL: expected ≥1 gui_input_poll( in demo_live, got $poll_count" >&2
  exit 1
fi

# Gap: take_* still inline alongside poll.
if ! grep -q 'glfw_gl_take_scroll_y(' "$DEMO"; then
  echo "[editor frame_input_red] FAIL: missing glfw_gl_take_scroll_y( in demo_live (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor frame_input_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok frame_input$' "$STABLE"; then
  echo "[editor frame_input_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor frame_input_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok frame_input_red"
echo "[editor frame_input_red] ok polls=$poll_count" >&2
