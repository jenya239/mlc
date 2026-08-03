#!/usr/bin/env bash
# TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE — L2 + demo wire (via EditorFrameLayout §97b).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/shared_syntax_span_cache_stable.mlc"
OUT_DIR="${UX_SHARED_SYNTAX_SPAN_CACHE_STABLE_OUT:-$ROOT_DIR/tmp/ux_shared_syntax_span_cache_stable}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if ! grep -q 'frame_layout_tick_spans(' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: demo missing frame_layout_tick_spans(" >&2
  exit 1
fi
if grep -q 'let mut shared_span_cache = editor_ux_syntax_span_cache_new()' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: demo still has bare shared_span_cache" >&2
  exit 1
fi
if grep -q 'let mut syntax_span_cache = editor_ux_syntax_span_cache_new()' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: demo still has syntax_span_cache" >&2
  exit 1
fi
if grep -q 'let mut minimap_span_cache = editor_ux_syntax_span_cache_new()' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: demo still has minimap_span_cache" >&2
  exit 1
fi
if grep -q 'syntax_span_cache, draw_text, range_start, range_end' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: demo still visible-range ticks" >&2
  exit 1
fi
if ! grep -q 'frame_layout, document.version, draw_text, 0, draw_text.byte_size()' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: demo missing full-buffer span tick via layout" >&2
  exit 1
fi
if ! grep -q 'const minimap_spans = frame_layout.span_cache.spans' "$DEMO"; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: minimap not reading layout span_cache" >&2
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
  echo "[ux shared_syntax_span_cache_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok shared_syntax_span_cache_stable'; then
  echo "[ux shared_syntax_span_cache_stable] FAIL: missing ux_ok shared_syntax_span_cache_stable" >&2
  exit 1
fi
echo "[ux shared_syntax_span_cache_stable] ok" >&2
