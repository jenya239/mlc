#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107j — single visible-row collect per content frame (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"

fail() {
  echo "[ux visible_rows_single_collect] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PERF" ] || fail "missing $PERF"

collect_count="$(
  grep -c 'collect_visible_visual_rows_pixel_budget_cached(' "$DEMO" || true
)"
if [ "$collect_count" -ne 1 ]; then
  fail "demo_live collect sites=$collect_count (expected 1)"
fi

if ! grep -q 'visible_collect_count' "$PERF"; then
  fail "visible_collect_count missing in perf.mlc"
fi
if ! grep -q 'visible_collect_count' "$DEMO"; then
  fail "demo_live not wiring visible_collect_count"
fi
if ! grep -q 'frame_visible_collect_count' "$DEMO"; then
  fail "demo_live missing frame_visible_collect_count"
fi

echo "ux_ok visible_rows_single_collect"
echo "[ux visible_rows_single_collect] ok collect_sites=1" >&2
