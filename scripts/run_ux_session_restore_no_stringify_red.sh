#!/usr/bin/env bash
# TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY STEP=1 — session restore still stringifies.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SESSION="$ROOT_DIR/misc/editor/workspace/session.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_session_restore_no_stringify_stable.sh"

if [ ! -f "$SESSION" ]; then
  echo "[ux session_restore_no_stringify_red] FAIL: missing $SESSION" >&2
  exit 1
fi

# Gap: workspace_session_restore_tabs still calls document_to_string (full flatten).
count="$(grep -c 'document_to_string' "$SESSION" || true)"
if [ "$count" -lt 1 ]; then
  echo "[ux session_restore_no_stringify_red] FAIL: expected ≥1 document_to_string in session.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn workspace_session_restore_tabs(' "$SESSION"; then
  echo "[ux session_restore_no_stringify_red] FAIL: missing workspace_session_restore_tabs" >&2
  exit 1
fi
if ! grep -q 'document_to_string(buffer.document)' "$SESSION"; then
  echo "[ux session_restore_no_stringify_red] FAIL: missing restore stringify call" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux session_restore_no_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok session_restore_no_stringify' "$STABLE"; then
  echo "[ux session_restore_no_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux session_restore_no_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok session_restore_no_stringify_red"
echo "[ux session_restore_no_stringify_red] ok" >&2
