#!/usr/bin/env bash
# TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY STEP=1 — idle paint-only still primary only.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_idle_multi_caret_overlay_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: idle paint-only blink overlays primary last_caret_* only.
if ! grep -q 'Paint-only blink' "$DEMO"; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: demo_live missing Paint-only blink" >&2
  exit 1
fi
if ! grep -q 'last_caret_x' "$DEMO"; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: demo_live missing last_caret_x" >&2
  exit 1
fi
# Paint-only block must still draw via last_caret_* (not multi cache yet).
if ! grep -A25 'Paint-only blink' "$DEMO" | grep -q 'last_caret_x'; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: paint-only block missing last_caret_x" >&2
  exit 1
fi
# No idle multi-caret overlay cache / loop yet.
if grep -qE 'idle_multi_caret_overlay|last_caret_rects|caret_overlay_count' "$DEMO"; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: demo_live already has idle multi overlay (expected gap)" >&2
  exit 1
fi
if grep -A40 'Paint-only blink' "$DEMO" | grep -qE 'while .*caret|multi_selection_count|last_carets'; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: paint-only already walks multi carets (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok idle_multi_caret_overlay_stable|idle multi caret overlay ok' "$STABLE"; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux idle_multi_caret_overlay_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok idle_multi_caret_overlay_red"
echo "[ux idle_multi_caret_overlay_red] ok" >&2
