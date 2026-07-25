#!/usr/bin/env bash
# TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY STEP=1 — tree click still stringifies.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
TREE_HIT="$ROOT_DIR/misc/editor/ux/tree_hit.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_tree_click_no_stringify_stable.sh"

if [ ! -f "$TREE_HIT" ]; then
  echo "[ux tree_click_no_stringify_red] FAIL: missing $TREE_HIT" >&2
  exit 1
fi

if ! grep -q 'export fn editor_ux_click_tree(' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_red] FAIL: missing editor_ux_click_tree" >&2
  exit 1
fi

# Gap: signature still takes open_text: string.
if ! grep -q 'open_text: string' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_red] FAIL: expected open_text: string param (gap)" >&2
  exit 1
fi

# Gap: still opens via tab_set_open(..., open_text).
if ! grep -q 'tab_set_open(state.tabs, node.path, open_text)' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_red] FAIL: expected tab_set_open(..., open_text)" >&2
  exit 1
fi

# Must not already use tab_set_open_buffer in tree_hit.
if grep -q 'tab_set_open_buffer' "$TREE_HIT"; then
  echo "[ux tree_click_no_stringify_red] FAIL: tab_set_open_buffer already present (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux tree_click_no_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok tree_click_no_stringify' "$STABLE"; then
  echo "[ux tree_click_no_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux tree_click_no_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok tree_click_no_stringify_red"
echo "[ux tree_click_no_stringify_red] ok" >&2
