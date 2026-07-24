#!/usr/bin/env bash
# TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE — L2 + demo wire for span cache.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/syntax_highlight_cache_stable.mlc"
OUT_DIR="${UX_SYNTAX_HIGHLIGHT_CACHE_STABLE_OUT:-$ROOT_DIR/tmp/ux_syntax_highlight_cache_stable}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
CACHE_MOD="$ROOT_DIR/misc/editor/ux/syntax_span_cache.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux syntax_highlight_cache_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$CACHE_MOD" ]; then
  echo "[ux syntax_highlight_cache_stable] FAIL: missing $CACHE_MOD" >&2
  exit 1
fi
if ! grep -q 'export fn editor_ux_syntax_span_cache_tick' "$CACHE_MOD"; then
  echo "[ux syntax_highlight_cache_stable] FAIL: missing cache tick helper" >&2
  exit 1
fi
if ! grep -q 'editor_ux_syntax_span_cache_tick' "$DEMO"; then
  echo "[ux syntax_highlight_cache_stable] FAIL: demo missing span cache tick" >&2
  exit 1
fi
if grep -q 'syntax_spans = highlight_range(draw_text, range_start, range_end)' "$DEMO"; then
  echo "[ux syntax_highlight_cache_stable] FAIL: demo still bare highlight_range assign" >&2
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
  echo "[ux syntax_highlight_cache_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok syntax_highlight_cache_stable'; then
  echo "[ux syntax_highlight_cache_stable] FAIL: missing ux_ok syntax_highlight_cache_stable" >&2
  exit 1
fi
echo "[ux syntax_highlight_cache_stable] ok" >&2
