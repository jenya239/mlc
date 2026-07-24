#!/usr/bin/env bash
# TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT STEP=1 — probe still stringifies document.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROBE="$ROOT_DIR/misc/editor/ux/probe.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_probe_reuses_line_index_text_stable.sh"

if [ ! -f "$PROBE" ]; then
  echo "[ux probe_reuses_line_index_text_red] FAIL: missing $PROBE" >&2
  exit 1
fi

# Gap: editor_ux_snapshot still calls document_to_string (full flatten).
count="$(grep -c 'document_to_string(active.buffer.document)' "$PROBE" || true)"
if [ "$count" -lt 1 ]; then
  echo "[ux probe_reuses_line_index_text_red] FAIL: expected ≥1 document_to_string call in probe.mlc, got $count" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_snapshot(' "$PROBE"; then
  echo "[ux probe_reuses_line_index_text_red] FAIL: missing editor_ux_snapshot" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux probe_reuses_line_index_text_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok probe_reuses_line_index_text' "$STABLE"; then
  echo "[ux probe_reuses_line_index_text_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux probe_reuses_line_index_text_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok probe_reuses_line_index_text_red"
echo "[ux probe_reuses_line_index_text_red] ok" >&2
