#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97c STEP=2 — zero loop-head app unpack.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[editor app_unpack_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

head_region="$(awk '/while glfw_gl_context_should_close/,/const scaled_font_size/' "$DEMO")"
unpack_count="$(printf '%s\n' "$head_region" | grep -c 'let mut .*= app\.' || true)"
if [ "$unpack_count" -ne 0 ]; then
  echo "[editor app_unpack_stable] FAIL: expected 0 loop-head unpacks, got $unpack_count" >&2
  exit 1
fi

for field in tabs selection history clipboard overlay editor_focused scroll_offset_y \
  mouse_was_down backspace_was_down enter_was_down selecting_text frame_index last_command; do
  if printf '%s\n' "$head_region" | grep -q "let mut ${field} = app"; then
    echo "[editor app_unpack_stable] FAIL: unpack of $field still at loop head" >&2
    exit 1
  fi
done

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok editor_app_unpack"
echo "[editor app_unpack_stable] ok unpacks=0" >&2
