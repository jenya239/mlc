#!/usr/bin/env bash
# TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY — L2: tree click via tab_set_open_buffer.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
TREE_HIT="$ROOT_DIR/misc/editor/ux/tree_hit.mlc"

if [ ! -f "$TREE_HIT" ]; then
  echo "[ux tree_click_no_stringify_stable] FAIL: missing $TREE_HIT" >&2
  exit 1
fi

if ! grep -q 'export fn editor_ux_click_tree(' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_stable] FAIL: missing editor_ux_click_tree" >&2
  exit 1
fi
if grep -q 'open_text: string' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_stable] FAIL: still has open_text: string" >&2
  exit 1
fi
if ! grep -q 'open_buffer: OpenBuffer' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_stable] FAIL: missing open_buffer: OpenBuffer" >&2
  exit 1
fi
if ! grep -q 'tab_set_open_buffer' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_stable] FAIL: missing tab_set_open_buffer" >&2
  exit 1
fi
if grep -q 'tab_set_open(state.tabs, node.path, open_text)' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_stable] FAIL: still uses tab_set_open(..., open_text)" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_ux_tree_click.sh"
bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok tree_click_no_stringify"
echo "[ux tree_click_no_stringify_stable] ok" >&2
