#!/usr/bin/env bash
# TRACK_EDITOR_SCROLLBAR_THEME_TINT STEP=1 — thumbs still hardcoded RGB.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_scrollbar_theme_tint_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux scrollbar_theme_tint_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'folder_scrollbar_thumb' "$DEMO"; then
  echo "[ux scrollbar_theme_tint_red] FAIL: missing folder thumb" >&2
  exit 1
fi
if ! grep -q 'editor_ux_content_scrollbar_thumb' "$DEMO"; then
  echo "[ux scrollbar_theme_tint_red] FAIL: missing content Y thumb" >&2
  exit 1
fi
if ! grep -q 'editor_ux_content_scrollbar_thumb_x' "$DEMO"; then
  echo "[ux scrollbar_theme_tint_red] FAIL: missing content X thumb" >&2
  exit 1
fi

# Gap: three thumb draw sites still hardcode 0.45/0.48/0.55.
hardcoded_count="$(grep -c '0.45, 0.48, 0.55' "$DEMO" || true)"
if [ "$hardcoded_count" -lt 3 ]; then
  echo "[ux scrollbar_theme_tint_red] FAIL: expected ≥3 hardcoded thumb tints, got $hardcoded_count" >&2
  exit 1
fi

# Content Y/X draw blocks must not already use theme.muted_*.
if grep -A12 'const content_thumb = editor_ux_content_scrollbar_thumb' "$DEMO" | grep -q 'theme.muted_'; then
  echo "[ux scrollbar_theme_tint_red] FAIL: content Y already uses theme.muted_ (expected gap)" >&2
  exit 1
fi
if grep -A12 'const content_thumb_x = editor_ux_content_scrollbar_thumb_x' "$DEMO" | grep -q 'theme.muted_'; then
  echo "[ux scrollbar_theme_tint_red] FAIL: content X already uses theme.muted_ (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux scrollbar_theme_tint_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok scrollbar_theme_tint_stable' "$STABLE"; then
  echo "[ux scrollbar_theme_tint_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux scrollbar_theme_tint_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok scrollbar_theme_tint_red"
echo "[ux scrollbar_theme_tint_red] ok" >&2
