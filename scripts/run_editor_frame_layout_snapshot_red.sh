#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b frame_cache STEP=1 — snapshot ticks still scattered.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_frame_layout_snapshot_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_snapshot_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if [ ! -f "$LAYOUT" ]; then
  echo "[editor frame_layout_snapshot_red] FAIL: missing $LAYOUT (wrap slice prerequisite)" >&2
  exit 1
fi

# Gap: no snapshot API on frame_layout yet.
if grep -q 'frame_layout_tick_snapshot' "$LAYOUT"; then
  echo "[editor frame_layout_snapshot_red] FAIL: frame_layout_tick_snapshot already present (expected gap)" >&2
  exit 1
fi
if grep -q 'frame_layout_tick_snapshot' "$DEMO"; then
  echo "[editor frame_layout_snapshot_red] FAIL: demo_live already uses frame_layout_tick_snapshot (expected gap)" >&2
  exit 1
fi

# Gap: many direct frame_snapshot_cache_tick( call sites in demo_live.
tick_count="$(grep -c 'frame_snapshot_cache_tick(' "$DEMO" || true)"
if [ "$tick_count" -lt 10 ]; then
  echo "[editor frame_layout_snapshot_red] FAIL: expected ≥10 frame_snapshot_cache_tick( calls, got $tick_count" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor frame_layout_snapshot_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok frame_layout_snapshot$' "$STABLE"; then
  echo "[editor frame_layout_snapshot_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor frame_layout_snapshot_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok frame_layout_snapshot_red"
echo "[editor frame_layout_snapshot_red] ok ticks=$tick_count" >&2
