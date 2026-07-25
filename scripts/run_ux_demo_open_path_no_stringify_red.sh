#!/usr/bin/env bash
# TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY STEP=1 — disk opens still stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_demo_open_path_no_stringify_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if ! grep -q 'fn try_open_path(' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: missing try_open_path" >&2
  exit 1
fi

# Gap: disk opens still use tab_set_open(..., text_for_path(...)).
count="$(grep -c 'tab_set_open([^,]*,[^,]*, text_for_path(' "$DEMO" || true)"
if [ "$count" -lt 2 ]; then
  # fallback: literal pattern count
  count="$(grep -c 'text_for_path(' "$DEMO" || true)"
fi
# Must have at least the try_open_path + tree_open sites via tab_set_open+text_for_path.
open_with_text="$(grep -c 'tab_set_open(.*, text_for_path(' "$DEMO" || true)"
if [ "$open_with_text" -lt 2 ]; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: expected ≥2 tab_set_open(..., text_for_path(...)), got $open_with_text" >&2
  exit 1
fi
if grep -q 'tab_set_open_buffer' "$DEMO"; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: tab_set_open_buffer already present (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok demo_open_path_no_stringify' "$STABLE"; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux demo_open_path_no_stringify_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok demo_open_path_no_stringify_red"
echo "[ux demo_open_path_no_stringify_red] ok" >&2
