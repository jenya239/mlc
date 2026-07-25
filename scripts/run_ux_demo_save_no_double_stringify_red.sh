#!/usr/bin/env bash
# TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY STEP=1 — demo save double-stringifies.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_demo_save_no_double_stringify_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux demo_save_no_double_stringify_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'fn save_demo_session(' "$DEMO"; then
  echo "[ux demo_save_no_double_stringify_red] FAIL: missing save_demo_session" >&2
  exit 1
fi

# Gap: save_demo_session calls document_to_string twice per tab.
count="$(grep -c 'document_to_string(tab.buffer.document)' "$DEMO" || true)"
if [ "$count" -lt 2 ]; then
  echo "[ux demo_save_no_double_stringify_red] FAIL: expected ≥2 document_to_string(tab.buffer.document) in demo_live.mlc, got $count" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux demo_save_no_double_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok demo_save_no_double_stringify' "$STABLE"; then
  echo "[ux demo_save_no_double_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux demo_save_no_double_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok demo_save_no_double_stringify_red"
echo "[ux demo_save_no_double_stringify_red] ok" >&2
