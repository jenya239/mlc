#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108c — Red: present_only caret ungated / no chrome retain.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_present_only_caret_no_chrome_rebuild.sh + scenario + counter.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_present_only_caret_no_chrome_rebuild.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/present_only_caret_no_chrome_rebuild.mlc"

fail() {
  echo "[ux present_only_caret_no_chrome_rebuild_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PERF" ] || fail "missing $PERF"

# Sanity: idle caret present_only early path + chrome bands exist (§106 / §107q).
if ! grep -q 'caret_blink_paint_dirty' "$DEMO"; then
  fail "demo_live missing caret_blink_paint_dirty (unexpected drift)"
fi
if ! grep -q 'editor_paint_ops_chrome_bands' "$DEMO"; then
  fail "demo_live missing editor_paint_ops_chrome_bands (unexpected drift)"
fi
if ! grep -q 'text_layer_rebuild_count' "$PERF"; then
  fail "perf missing text_layer_rebuild_count (unexpected drift)"
fi

# Red: no chrome_rebuild_count yet.
if grep -q 'chrome_rebuild_count' "$PERF"; then
  fail "chrome_rebuild_count already present (expected gap until Green)"
fi
if grep -q 'editor_perf_note_chrome_rebuild' "$PERF" "$DEMO"; then
  fail "note_chrome_rebuild already present (expected gap until Green)"
fi

# Red: no retained chrome EditorPaintOp batch / replay helper.
if grep -Eq 'chrome_layer_batch|ChromeLayerBatch|retained_chrome_ops|editor_ux_chrome_layer' "$DEMO"; then
  fail "chrome layer retain already present (expected gap until Green)"
fi

# Red: chrome bands still rebuilt every full paint (no retain/replay gate).
chrome_call_count="$(grep -c 'editor_paint_ops_chrome_bands' "$DEMO" || true)"
if [ "$chrome_call_count" -lt 1 ]; then
  fail "demo_live no longer builds chrome bands (unexpected drift)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "present_only caret ungated; chrome ops rebuilt each paint (Red expected)"
