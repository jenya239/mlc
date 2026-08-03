#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107i — hover / layout_skip skips spans retick (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/hover_no_full_compare.mlc"
OUT_DIR="${UX_HOVER_NO_FULL_COMPARE_OUT:-$ROOT_DIR/tmp/ux_hover_no_full_compare}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux hover_no_full_compare] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

spans_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("frame_layout = frame_layout_tick_spans(")
    abort "missing frame_layout_tick_spans call" if start_at.nil?
    window_start = source.rindex("\n    if ", start_at)
    abort "missing enclosing if" if window_start.nil?
    window_end = source.index("\n    end\n", start_at)
    abort "missing enclosing end" if window_end.nil?
    print source[window_start...window_end]
  ' "$DEMO"
)"
if ! printf '%s\n' "$spans_block" | grep -q 'layout_skip'; then
  echo "[ux hover_no_full_compare] FAIL: spans tick not gated by layout_skip" >&2
  exit 1
fi
if ! printf '%s\n' "$spans_block" | grep -q 'perf_skip_heavy'; then
  echo "[ux hover_no_full_compare] FAIL: spans tick missing perf_skip_heavy" >&2
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
  echo "[ux hover_no_full_compare] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok hover_no_full_compare'; then
  echo "[ux hover_no_full_compare] FAIL: missing ux_ok" >&2
  exit 1
fi
echo "[ux hover_no_full_compare] ok" >&2
