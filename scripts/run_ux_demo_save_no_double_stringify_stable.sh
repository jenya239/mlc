#!/usr/bin/env bash
# TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY — L2: one flatten per tab in save_demo_session.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -f "$DEMO" ]; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'fn save_demo_session(' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: missing save_demo_session" >&2
  exit 1
fi

count="$(grep -c 'document_to_string(tab.buffer.document)' "$DEMO" || true)"
if [ "$count" -ne 1 ]; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: expected exactly 1 document_to_string(tab.buffer.document), got $count" >&2
  exit 1
fi
if ! grep -q 'const text = document_to_string(tab.buffer.document)' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: missing single-bound text flatten" >&2
  exit 1
fi
if ! grep -q 'dump_tab_file(tab.path, text)' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: dump_tab_file must reuse text" >&2
  exit 1
fi
if ! grep -q 'tab_set_open(saved, out_path, text)' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: tab_set_open must reuse text" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok demo_save_no_double_stringify"
echo "[ux demo_save_no_double_stringify_stable] ok" >&2
