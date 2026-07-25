#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b dual-wrap STEP=1 — two wrap ticks still in demo_live.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_frame_layout_dual_wrap_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_dual_wrap_red] FAIL: missing $DEMO" >&2
  exit 1
fi

tick_count="$(grep -c 'frame_layout_tick_pixel(' "$DEMO" || true)"
if [ "$tick_count" -lt 2 ]; then
  echo "[editor frame_layout_dual_wrap_red] FAIL: expected ≥2 frame_layout_tick_pixel( calls, got $tick_count" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor frame_layout_dual_wrap_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok frame_layout_dual_wrap$' "$STABLE"; then
  echo "[editor frame_layout_dual_wrap_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor frame_layout_dual_wrap_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok frame_layout_dual_wrap_red"
echo "[editor frame_layout_dual_wrap_red] ok ticks=$tick_count" >&2
