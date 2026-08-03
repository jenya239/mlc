#!/usr/bin/env bash
# ARCH-LINT: source-shape / ownership only (TRACK_EDITOR_HYGIENE §107r). Not a behavioural UX scenario.
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b frame_cache STEP=2 — snapshot owned by frame_layout.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"

if [ ! -f "$LAYOUT" ]; then
  echo "[editor frame_layout_snapshot_stable] FAIL: missing $LAYOUT" >&2
  exit 1
fi
if ! grep -q 'export fn frame_layout_tick_snapshot' "$LAYOUT"; then
  echo "[editor frame_layout_snapshot_stable] FAIL: missing frame_layout_tick_snapshot" >&2
  exit 1
fi
if ! grep -q 'snapshot_cache: DocumentFrameSnapshotCache' "$LAYOUT"; then
  echo "[editor frame_layout_snapshot_stable] FAIL: EditorFrameLayout missing snapshot_cache" >&2
  exit 1
fi

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_snapshot_stable] FAIL: missing $DEMO" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_snapshot(' "$DEMO"; then
  echo "[editor frame_layout_snapshot_stable] FAIL: demo_live missing frame_layout_tick_snapshot(" >&2
  exit 1
fi

tick_count="$(grep -c 'frame_snapshot_cache_tick(' "$DEMO" || true)"
if [ "$tick_count" -ne 0 ]; then
  echo "[editor frame_layout_snapshot_stable] FAIL: demo_live still has frame_snapshot_cache_tick( (count=$tick_count)" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok frame_layout_snapshot"
echo "[editor frame_layout_snapshot_stable] ok" >&2
