#!/usr/bin/env bash
# TRACK_EDITOR_PROPORTIONAL_HIT_TEST STEP=1 — hit/caret/selection still mono cell.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
WRAP="$ROOT_DIR/misc/editor/layout/word_wrap.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_proportional_hit_test_stable.sh"

if [ ! -f "$WRAP" ]; then
  echo "[ux proportional_hit_test_red] FAIL: missing $WRAP" >&2
  exit 1
fi
if [ ! -f "$DEMO" ]; then
  echo "[ux proportional_hit_test_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: click hit-test still mono cell (column_in_row = content_x / cell_width).
if ! grep -q 'export fn wrap_offset_at_point' "$WRAP"; then
  echo "[ux proportional_hit_test_red] FAIL: wrap_offset_at_point missing" >&2
  exit 1
fi
if ! grep -q 'column_in_row = content_x / cell_width' "$WRAP"; then
  echo "[ux proportional_hit_test_red] FAIL: wrap_offset_at_point not mono cell divide" >&2
  exit 1
fi
# No advance-sum hit API yet (wrap budget advances exist; hit path must not).
if grep -qE 'wrap_offset_at_point_pixel|hit_byte_at_pixel_advance|pixel_hit_test_advance' "$WRAP"; then
  echo "[ux proportional_hit_test_red] FAIL: word_wrap already has pixel hit-test (expected gap)" >&2
  exit 1
fi
# demo_live caret still mono cell.
if ! grep -q 'caret_col \* metrics.char_width' "$DEMO"; then
  echo "[ux proportional_hit_test_red] FAIL: demo_live caret not mono char_width" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[ux proportional_hit_test_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok proportional_hit_test_stable|proportional hit-test ok' "$STABLE"; then
  echo "[ux proportional_hit_test_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux proportional_hit_test_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok proportional_hit_test_red"
echo "[ux proportional_hit_test_red] ok" >&2
