#!/usr/bin/env bash
# TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE — L2 + demo wire for minimap span cache.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/minimap_highlight_cache_stable.mlc"
OUT_DIR="${UX_MINIMAP_HIGHLIGHT_CACHE_STABLE_OUT:-$ROOT_DIR/tmp/ux_minimap_highlight_cache_stable}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux minimap_highlight_cache_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if ! grep -q 'frame_layout.span_cache.spans' "$DEMO"; then
  echo "[ux minimap_highlight_cache_stable] FAIL: demo missing layout span_cache for minimap" >&2
  exit 1
fi
if grep -q 'const minimap_spans = highlight_mlc(draw_text)' "$DEMO"; then
  echo "[ux minimap_highlight_cache_stable] FAIL: demo still bare highlight_mlc(draw_text)" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_spans(' "$DEMO"; then
  echo "[ux minimap_highlight_cache_stable] FAIL: demo missing frame_layout_tick_spans(" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux minimap_highlight_cache_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok minimap_highlight_cache_stable'; then
  echo "[ux minimap_highlight_cache_stable] FAIL: missing ux_ok minimap_highlight_cache_stable" >&2
  exit 1
fi
echo "[ux minimap_highlight_cache_stable] ok" >&2
