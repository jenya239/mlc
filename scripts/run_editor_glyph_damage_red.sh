#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110e — Red: no glyph-damage harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_glyph_damage.sh + scroll Y-adjust / newly-visible reshape.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_glyph_damage.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STATIC_TEXT="$ROOT_DIR/misc/editor/ui/static_text.mlc"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
GLYPH_LAYER="$ROOT_DIR/scripts/run_editor_perf_glyph_layer_budget.sh"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"

fail() {
  echo "[editor_glyph_damage_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$STATIC_TEXT" ] || fail "missing static_text.mlc (unexpected drift)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$GLYPH_LAYER" ] || fail "missing glyph-layer harness (unexpected drift — §109e required)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green glyph-damage harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_glyph_damage.sh already present (expected gap until Green)"
fi

# Red: editor glyph fingerprint still keys scroll_offset_y (forces full rebuild).
if ! grep -n 'editor_glyph_fp' -A20 "$DEMO" | grep -q 'scroll_offset_y'; then
  fail "demo_live editor_glyph_fp no longer includes scroll_offset_y (expected gap until Green)"
fi

# Red: no Y-adjust / newly-visible glyph damage API yet.
if grep -E 'static_text_glyph_batch_adjust_y|glyph_batch_adjust_y|newly_visible_rows|editor_glyph_damage' \
  "$STATIC_TEXT" "$DEMO" >/dev/null 2>&1; then
  fail "sources already declare glyph Y-adjust/newly-visible helpers (expected until Green)"
fi

# Red: dogfood scroll ceiling still §109 default 60 (not yet tightened <60).
if ! grep -q 'SCROLL_CPU_MAX="${MLC_EDITOR_PERF_SCROLL_CPU_MAX:-60}"' "$DOGFOOD_GATE"; then
  fail "dogfood SCROLL_CPU_MAX default is not 60 (expected gap until Green tightens <60)"
fi

# Red: ux_gate still does not wire glyph-damage harness.
if grep -q 'run_editor_glyph_damage\|glyph_damage\|newly_visible_rows' "$UX_GATE"; then
  fail "run_ux_gate already wires glyph damage (Decision: not folded)"
fi

# Red: no §110e Green measured paste yet.
if grep -A40 '### §110e Green measured\|Green measured (§110e)\|### Green measured (§110e)' "$TRACK" 2>/dev/null | \
  grep -Eq 'SCROLL_CPU_MAX=|scroll_cpu_percent=|editor_glyph_shape|newly_visible'; then
  fail "TRACK already has glyph-damage post-cut measured numbers (expected gap until Green)"
fi

fail "no editor glyph damage harness (Red expected)"
