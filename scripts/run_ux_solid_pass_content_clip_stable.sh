#!/usr/bin/env bash
# TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP — solid pass uses content_clip (green after STEP=2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux solid_pass_content_clip_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if grep -q 'editor_gl_scissor_enable_rect(height, editor_rect)' "$DEMO"; then
  echo "[ux solid_pass_content_clip_stable] FAIL: solid pass still scissors editor_rect (expected red until STEP=2)" >&2
  exit 1
fi
content_clip_count="$(grep -c 'editor_gl_scissor_enable_rect(height, content_clip)' "$DEMO" || true)"
if [ "$content_clip_count" -lt 2 ]; then
  echo "[ux solid_pass_content_clip_stable] FAIL: need solid+glyph content_clip scissors (count=$content_clip_count)" >&2
  exit 1
fi
if ! grep -q 'minimap_indicator' "$DEMO"; then
  echo "[ux solid_pass_content_clip_stable] FAIL: minimap_indicator missing" >&2
  exit 1
fi

echo "ux_ok solid_pass_content_clip_stable"
echo "[ux solid_pass_content_clip_stable] ok" >&2
