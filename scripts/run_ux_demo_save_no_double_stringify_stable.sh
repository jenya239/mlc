#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107b — session save stores original paths (no dump / no flatten).
# Supersedes TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY L2 (dump path deleted).
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

if ! grep -q 'workspace_session_save_tabs(' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: save_demo_session must call workspace_session_save_tabs" >&2
  exit 1
fi

if grep -q 'fn dump_tab_file(' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: dump_tab_file must be removed" >&2
  exit 1
fi

count="$(grep -c 'document_to_string(tab.buffer.document)' "$DEMO" || true)"
if [ "$count" -ne 0 ]; then
  echo "[ux demo_save_no_double_stringify_stable] FAIL: expected 0 document_to_string(tab.buffer.document), got $count" >&2
  exit 1
fi

bash "$ROOT_DIR/scripts/run_editor_demo_live_fs_compile.sh"

echo "ux_ok demo_save_no_double_stringify"
echo "[ux demo_save_no_double_stringify_stable] ok" >&2
