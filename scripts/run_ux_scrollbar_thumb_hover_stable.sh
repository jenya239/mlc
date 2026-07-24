#!/usr/bin/env bash
# TRACK_EDITOR_SCROLLBAR_THUMB_HOVER — L0: thumbs use chrome_hover_draw_entry.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux scrollbar_thumb_hover_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -A12 'content_thumb.width > 0 then' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux scrollbar_thumb_hover_stable] FAIL: content Y missing chrome_hover" >&2
  exit 1
fi
if ! grep -A12 'content_thumb_x.width > 0 then' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux scrollbar_thumb_hover_stable] FAIL: content X missing chrome_hover" >&2
  exit 1
fi
if ! grep -A12 'const thumb = folder_scrollbar_thumb' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux scrollbar_thumb_hover_stable] FAIL: folder thumb missing chrome_hover" >&2
  exit 1
fi

# Muted base still passed into chrome_hover.
if ! grep -A12 'content_thumb.width > 0 then' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_thumb_hover_stable] FAIL: content Y missing muted base" >&2
  exit 1
fi

echo "ux_ok scrollbar_thumb_hover_stable"
echo "[ux scrollbar_thumb_hover_stable] ok" >&2
