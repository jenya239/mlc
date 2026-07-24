#!/usr/bin/env bash
# TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE STEP=1 — no span cache; bare highlight_range each paint.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_syntax_highlight_cache_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux syntax_highlight_cache_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: paint path calls highlight_range directly; no span-cache helper.
if ! grep -q 'highlight_range(draw_text, range_start, range_end)' "$DEMO"; then
  echo "[ux syntax_highlight_cache_red] FAIL: demo missing bare highlight_range call" >&2
  exit 1
fi
if grep -qE 'editor_ux_syntax_span_cache|syntax_span_cache' "$DEMO"; then
  echo "[ux syntax_highlight_cache_red] FAIL: span cache already in demo (expected gap)" >&2
  exit 1
fi
if find "$ROOT_DIR/misc/editor" -name '*.mlc' -print0 | xargs -0 grep -l 'export fn editor_ux_syntax_span_cache' 2>/dev/null | grep -q .; then
  echo "[ux syntax_highlight_cache_red] FAIL: span cache helper already exists (expected gap)" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux syntax_highlight_cache_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok syntax_highlight_cache_stable' "$STABLE"; then
  echo "[ux syntax_highlight_cache_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux syntax_highlight_cache_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok syntax_highlight_cache_red"
echo "[ux syntax_highlight_cache_red] ok" >&2
