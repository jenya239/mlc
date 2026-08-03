#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107h — long single-line shape budget (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/long_single_line_budget.mlc"
OUT_DIR="${UX_LONG_SINGLE_LINE_BUDGET_OUT:-$ROOT_DIR/tmp/ux_long_single_line_budget}"
BIN_OUT="$OUT_DIR/bin"
WRAP="$ROOT_DIR/misc/editor/layout/word_wrap.mlc"
# Written after first honest Green measurement: elapsed_us=121158 → TOTAL_US_MAX=500000 (~4×).
TOTAL_US_MAX="${MLC_EDITOR_LONG_LINE_SHAPE_TOTAL_US_MAX:-500000}"

if [ ! -x "$MLCC" ]; then
  echo "[ux long_single_line_budget] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

advances_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("fn line_codepoint_advances_px(")
    abort "missing line_codepoint_advances_px" if start_at.nil?
    end_at = source.index("\nfn ", start_at + 1)
    abort "missing next fn" if end_at.nil?
    print source[start_at...end_at]
  ' "$WRAP"
)"

if ! grep -q 'SHAPE_LINE_BYTE_CAP' "$WRAP"; then
  echo "[ux long_single_line_budget] FAIL: SHAPE_LINE_BYTE_CAP missing" >&2
  exit 1
fi
if ! grep -q 'SHAPE_SEGMENT_CODEPOINT_MAX' "$WRAP"; then
  echo "[ux long_single_line_budget] FAIL: SHAPE_SEGMENT_CODEPOINT_MAX missing" >&2
  exit 1
fi
if printf '%s\n' "$advances_block" | grep -q 'shape each codepoint alone'; then
  echo "[ux long_single_line_budget] FAIL: per-codepoint-alone mismatch loop still present" >&2
  exit 1
fi
if ! printf '%s\n' "$advances_block" | grep -q 'SHAPE_LINE_BYTE_CAP'; then
  echo "[ux long_single_line_budget] FAIL: advances path ignores SHAPE_LINE_BYTE_CAP" >&2
  exit 1
fi
if ! printf '%s\n' "$advances_block" | grep -q 'SHAPE_SEGMENT_CODEPOINT_MAX'; then
  echo "[ux long_single_line_budget] FAIL: advances path ignores SHAPE_SEGMENT_CODEPOINT_MAX" >&2
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
  echo "[ux long_single_line_budget] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok long_single_line_budget'; then
  echo "[ux long_single_line_budget] FAIL: missing ux_ok" >&2
  exit 1
fi
elapsed_us="$(
  printf '%s\n' "$output" | sed -n 's/^long_single_line_budget_us=//p' | head -1
)"
if [ -z "$elapsed_us" ]; then
  echo "[ux long_single_line_budget] FAIL: missing long_single_line_budget_us" >&2
  exit 1
fi
if [ "$elapsed_us" -gt "$TOTAL_US_MAX" ]; then
  echo "[ux long_single_line_budget] FAIL: elapsed_us=$elapsed_us > TOTAL_US_MAX=$TOTAL_US_MAX" >&2
  exit 1
fi
echo "[ux long_single_line_budget] ok elapsed_us=$elapsed_us max=$TOTAL_US_MAX" >&2
