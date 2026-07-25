#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97c STEP=1 — live loop still unpacks EditorAppState.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_app_unpack_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor app_unpack_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: loop-head unpack of app shell fields (≥8).
unpack_count="$(
  awk '/while glfw_gl_context_should_close/,/const scaled_font_size/' "$DEMO" \
    | grep -c 'let mut .*= app\.' || true
)"
if [ "$unpack_count" -lt 8 ]; then
  echo "[editor app_unpack_red] FAIL: expected ≥8 let mut = app. unpacks at loop head, got $unpack_count" >&2
  exit 1
fi

# Named shell fields still present as unpacks.
for field in tabs selection history clipboard overlay editor_focused scroll_offset_y; do
  if ! awk '/while glfw_gl_context_should_close/,/const scaled_font_size/' "$DEMO" \
      | grep -q "let mut ${field} = app"; then
    echo "[editor app_unpack_red] FAIL: missing unpack of $field at loop head (expected gap)" >&2
    exit 1
  fi
done

if [ ! -f "$STABLE" ]; then
  echo "[editor app_unpack_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok editor_app_unpack$' "$STABLE"; then
  echo "[editor app_unpack_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor app_unpack_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok editor_app_unpack_red"
echo "[editor app_unpack_red] ok unpacks=$unpack_count" >&2
