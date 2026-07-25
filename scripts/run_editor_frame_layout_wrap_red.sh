#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b STEP=1 — wrap still dual-inline; no frame_layout.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_frame_layout_wrap_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_wrap_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: frame_layout module not extracted yet.
if [ -f "$LAYOUT" ]; then
  echo "[editor frame_layout_wrap_red] FAIL: frame_layout.mlc already exists (expected gap)" >&2
  exit 1
fi

# Gap: demo_live still calls wrap_count_cache_tick_pixel inline (≥2 sites).
tick_count="$(grep -c 'wrap_count_cache_tick_pixel(' "$DEMO" || true)"
if [ "$tick_count" -lt 2 ]; then
  echo "[editor frame_layout_wrap_red] FAIL: expected ≥2 inline wrap_count_cache_tick_pixel( calls, got $tick_count" >&2
  exit 1
fi

# Gap: demo_live does not yet use frame_layout API.
if grep -qE 'frame_layout_|EditorFrameLayout|from '\''\./app/frame_layout'\''' "$DEMO"; then
  echo "[editor frame_layout_wrap_red] FAIL: demo_live already wires frame_layout (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor frame_layout_wrap_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok frame_layout_wrap$' "$STABLE"; then
  echo "[editor frame_layout_wrap_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor frame_layout_wrap_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok frame_layout_wrap_red"
echo "[editor frame_layout_wrap_red] ok" >&2
