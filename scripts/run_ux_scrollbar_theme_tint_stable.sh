#!/usr/bin/env bash
# TRACK_EDITOR_SCROLLBAR_THEME_TINT — L0: thumbs draw with theme.muted_*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux scrollbar_theme_tint_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if grep -q '0.45, 0.48, 0.55' "$DEMO"; then
  echo "[ux scrollbar_theme_tint_stable] FAIL: hardcoded 0.45/0.48/0.55 still present" >&2
  exit 1
fi

if ! grep -A12 'const thumb = folder_scrollbar_thumb' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_theme_tint_stable] FAIL: folder thumb missing theme.muted_" >&2
  exit 1
fi
if ! grep -A12 'const content_thumb = editor_ux_content_scrollbar_thumb' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_theme_tint_stable] FAIL: content Y thumb missing theme.muted_" >&2
  exit 1
fi
if ! grep -A12 'const content_thumb_x = editor_ux_content_scrollbar_thumb_x' "$DEMO" | grep -q 'theme.muted_red'; then
  echo "[ux scrollbar_theme_tint_stable] FAIL: content X thumb missing theme.muted_" >&2
  exit 1
fi

echo "ux_ok scrollbar_theme_tint_stable"
echo "[ux scrollbar_theme_tint_stable] ok" >&2
