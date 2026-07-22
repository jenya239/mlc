#!/usr/bin/env bash
# TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP STEP=1 — solid pass still scissors editor_rect.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux solid_pass_content_clip_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Glyph batch already uses content_clip (#37); solid pass must still use editor_rect.
if ! grep -q 'editor_gl_scissor_enable_rect(height, content_clip)' "$DEMO"; then
  echo "[ux solid_pass_content_clip_red] FAIL: demo_live missing glyph content_clip scissor" >&2
  exit 1
fi
if ! grep -q 'editor_gl_scissor_enable_rect(height, editor_rect)' "$DEMO"; then
  echo "[ux solid_pass_content_clip_red] FAIL: demo_live missing solid editor_rect scissor (expected gap)" >&2
  exit 1
fi
content_clip_count="$(grep -c 'editor_gl_scissor_enable_rect(height, content_clip)' "$DEMO" || true)"
if [ "$content_clip_count" -ge 2 ]; then
  echo "[ux solid_pass_content_clip_red] FAIL: solid already uses content_clip (expected gap)" >&2
  exit 1
fi

echo "ux_ok solid_pass_content_clip_red"
echo "[ux solid_pass_content_clip_red] ok" >&2
