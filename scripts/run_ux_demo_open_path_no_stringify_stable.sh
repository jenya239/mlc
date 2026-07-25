#!/usr/bin/env bash
# TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY — L2: disk opens via tab_set_open_buffer.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'tab_set_open_buffer' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: missing tab_set_open_buffer" >&2
  exit 1
fi
if ! grep -q 'open_buffer_from_path' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: missing open_buffer_from_path" >&2
  exit 1
fi
if ! grep -q 'fn try_open_path(' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: missing try_open_path" >&2
  exit 1
fi
if ! grep -q 'fn open_path_into_tabs(' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: missing open_path_into_tabs" >&2
  exit 1
fi
if ! grep -q 'open_path_into_tabs(app.ux.tabs, open_path)' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: tree_open must call open_path_into_tabs" >&2
  exit 1
fi

try_block="$(awk '/^fn try_open_path\(/,/^end$/' "$DEMO" | head -20)"
if printf '%s\n' "$try_block" | grep -q 'text_for_path'; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: try_open_path still uses text_for_path" >&2
  exit 1
fi
if ! printf '%s\n' "$try_block" | grep -q 'tab_set_open_buffer'; then
  echo "[ux demo_open_path_no_stringify_stable] FAIL: try_open_path missing tab_set_open_buffer" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok demo_open_path_no_stringify"
echo "[ux demo_open_path_no_stringify_stable] ok" >&2
