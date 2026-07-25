#!/usr/bin/env bash
# TRACK_EDITOR_DROP_FILE_NO_STRINGIFY STEP=1 — drop_file still stringifies.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DROP_FILE="$ROOT_DIR/misc/editor/ux/drop_file.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_drop_file_no_stringify_stable.sh"

if [ ! -f "$DROP_FILE" ]; then
  echo "[ux drop_file_no_stringify_red] FAIL: missing $DROP_FILE" >&2
  exit 1
fi

# Gap: editor_ux_open_dropped_path still calls document_to_string (full flatten).
count="$(grep -c 'document_to_string' "$DROP_FILE" || true)"
if [ "$count" -lt 1 ]; then
  echo "[ux drop_file_no_stringify_red] FAIL: expected ≥1 document_to_string in drop_file.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_open_dropped_path(' "$DROP_FILE"; then
  echo "[ux drop_file_no_stringify_red] FAIL: missing editor_ux_open_dropped_path" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux drop_file_no_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok drop_file_no_stringify' "$STABLE"; then
  echo "[ux drop_file_no_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux drop_file_no_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok drop_file_no_stringify_red"
echo "[ux drop_file_no_stringify_red] ok" >&2
