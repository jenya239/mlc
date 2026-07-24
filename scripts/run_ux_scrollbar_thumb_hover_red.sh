#!/usr/bin/env bash
# TRACK_EDITOR_SCROLLBAR_THUMB_HOVER STEP=1 — thumbs still flat muted fills.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_scrollbar_thumb_hover_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'editor_ux_content_scrollbar_thumb(' "$DEMO"; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: missing content Y thumb" >&2
  exit 1
fi
if ! grep -q 'editor_ux_content_scrollbar_thumb_x(' "$DEMO"; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: missing content X thumb" >&2
  exit 1
fi
if ! grep -q 'folder_scrollbar_thumb(' "$DEMO"; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: missing folder thumb" >&2
  exit 1
fi

# Gap: draw sites use flat theme.muted_* without chrome_hover near thumb rects.
if ! grep -A8 'content_thumb.width > 0 then' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: content Y missing flat muted fill" >&2
  exit 1
fi
if grep -A10 'content_thumb.width > 0 then' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: content Y already uses chrome_hover (expected gap)" >&2
  exit 1
fi

if ! grep -A8 'content_thumb_x.width > 0 then' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: content X missing flat muted fill" >&2
  exit 1
fi
if grep -A10 'content_thumb_x.width > 0 then' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: content X already uses chrome_hover (expected gap)" >&2
  exit 1
fi

if ! grep -A8 'thumb.width > 0 then' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: folder thumb missing flat muted fill" >&2
  exit 1
fi
if grep -B8 'thumb.x, thumb.y, thumb.width, thumb.height' "$DEMO" | grep -q 'editor_ux_chrome_hover_draw_entry'; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: folder thumb already uses chrome_hover (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok scrollbar_thumb_hover_stable' "$STABLE"; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux scrollbar_thumb_hover_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok scrollbar_thumb_hover_red"
echo "[ux scrollbar_thumb_hover_red] ok" >&2
