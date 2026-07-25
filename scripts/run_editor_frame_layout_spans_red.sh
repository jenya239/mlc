#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97b shared_span_cache STEP=1 — span cache still bare in demo_live.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
LAYOUT="$ROOT_DIR/misc/editor/app/frame_layout.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_frame_layout_spans_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor frame_layout_spans_red] FAIL: missing $DEMO" >&2
  exit 1
fi

if [ ! -f "$LAYOUT" ]; then
  echo "[editor frame_layout_spans_red] FAIL: missing $LAYOUT (prior slice prerequisite)" >&2
  exit 1
fi

# Gap: no span API on frame_layout yet.
if grep -q 'frame_layout_tick_spans' "$LAYOUT"; then
  echo "[editor frame_layout_spans_red] FAIL: frame_layout_tick_spans already present (expected gap)" >&2
  exit 1
fi
if grep -q 'frame_layout_tick_spans' "$DEMO"; then
  echo "[editor frame_layout_spans_red] FAIL: demo_live already uses frame_layout_tick_spans (expected gap)" >&2
  exit 1
fi
if grep -q 'span_cache: SyntaxSpanCacheState' "$LAYOUT"; then
  echo "[editor frame_layout_spans_red] FAIL: EditorFrameLayout already has span_cache (expected gap)" >&2
  exit 1
fi

# Gap: bare shared_span_cache local + direct tick in demo_live.
if ! grep -q 'let mut shared_span_cache = editor_ux_syntax_span_cache_new()' "$DEMO"; then
  echo "[editor frame_layout_spans_red] FAIL: missing bare shared_span_cache local (expected gap)" >&2
  exit 1
fi

tick_count="$(grep -c 'editor_ux_syntax_span_cache_tick(' "$DEMO" || true)"
if [ "$tick_count" -lt 1 ]; then
  echo "[editor frame_layout_spans_red] FAIL: expected ≥1 editor_ux_syntax_span_cache_tick( in demo_live, got $tick_count" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor frame_layout_spans_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok frame_layout_spans$' "$STABLE"; then
  echo "[editor frame_layout_spans_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor frame_layout_spans_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok frame_layout_spans_red"
echo "[editor frame_layout_spans_red] ok ticks=$tick_count" >&2
