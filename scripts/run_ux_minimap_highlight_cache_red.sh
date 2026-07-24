#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE STEP=1 — bare highlight_mlc on minimap rebuild.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_minimap_highlight_cache_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux minimap_highlight_cache_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: minimap rebuild re-lexes full buffer even on height/font-only rebuild.
if ! grep -q 'const minimap_spans = highlight_mlc(draw_text)' "$DEMO"; then
  echo "[ux minimap_highlight_cache_red] FAIL: demo missing bare highlight_mlc(draw_text)" >&2
  exit 1
fi
if grep -q 'minimap_span_cache' "$DEMO"; then
  echo "[ux minimap_highlight_cache_red] FAIL: minimap_span_cache already in demo (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux minimap_highlight_cache_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok minimap_highlight_cache_stable' "$STABLE"; then
  echo "[ux minimap_highlight_cache_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux minimap_highlight_cache_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok minimap_highlight_cache_red"
echo "[ux minimap_highlight_cache_red] ok" >&2
