#!/usr/bin/env bash
# TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE STEP=1 — dual visible-range + minimap caches.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_shared_syntax_span_cache_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux shared_syntax_span_cache_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: separate visible-range cache + full-buffer minimap cache.
if ! grep -q 'let mut syntax_span_cache = editor_ux_syntax_span_cache_new()' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_red] FAIL: missing syntax_span_cache binding" >&2
  exit 1
fi
if ! grep -q 'let mut minimap_span_cache = editor_ux_syntax_span_cache_new()' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_red] FAIL: missing minimap_span_cache binding" >&2
  exit 1
fi
if ! grep -q 'syntax_span_cache, draw_text, range_start, range_end' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_red] FAIL: missing visible-range tick" >&2
  exit 1
fi
if grep -q 'shared_span_cache' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_red] FAIL: shared_span_cache already present (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux shared_syntax_span_cache_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok shared_syntax_span_cache_stable' "$STABLE"; then
  echo "[ux shared_syntax_span_cache_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux shared_syntax_span_cache_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok shared_syntax_span_cache_red"
echo "[ux shared_syntax_span_cache_red] ok" >&2
