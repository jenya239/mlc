#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_CONTENT_DIRTY §111a0 STEP=1 Red — gate/wire absent on today's tree.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
GREEN="$ROOT_DIR/scripts/run_editor_scroll_content_dirty.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/scroll_content_dirty_forces_collect.mlc"

fail() {
  echo "[editor scroll content dirty red] FAIL: $*" >&2
  exit 1
}

if [ ! -f "$GREEN" ]; then
  fail "no editor scroll content dirty harness"
fi
if [ ! -f "$SCENARIO" ]; then
  fail "no scroll_content_dirty_forces_collect scenario"
fi
if ! grep -qE 'editor_scroll_frame_note_changed|editor_live_note_scroll_changed|editor_scroll_frame_after_offsets' "$DEMO"; then
  fail "demo_live missing scroll frame note wire"
fi

# Green present → this red must exit 1 with "already present" (Critic/Driver check).
echo "[editor scroll content dirty red] FAIL: already present" >&2
exit 1
