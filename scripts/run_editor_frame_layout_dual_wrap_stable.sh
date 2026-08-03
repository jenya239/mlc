#!/usr/bin/env bash
# ARCH-LINT: source-shape / ownership only (TRACK_EDITOR_HYGIENE §107r). Not a behavioural UX scenario.
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b dual-wrap STEP=2 — one late wrap tick per frame.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_dual_wrap_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

tick_count="$(grep -c 'frame_layout_tick_pixel(' "$DEMO" || true)"
if [ "$tick_count" -ne 1 ]; then
  echo "[editor frame_layout_dual_wrap_stable] FAIL: expected exactly 1 frame_layout_tick_pixel(, got $tick_count" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok frame_layout_dual_wrap"
echo "[editor frame_layout_dual_wrap_stable] ok ticks=$tick_count" >&2
