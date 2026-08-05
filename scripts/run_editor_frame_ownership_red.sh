#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110b — Red: no frame-ownership harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_frame_ownership.sh + EditorFrame generations.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_frame_ownership.sh"
EDITOR_DIR="$ROOT_DIR/misc/editor"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"

fail() {
  echo "[editor_frame_ownership_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -d "$EDITOR_DIR" ] || fail "missing misc/editor (unexpected drift)"
[ -f "$WAKE" ] || fail "missing wake harness (unexpected drift — §109b required)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green ownership harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_frame_ownership.sh already present (expected gap until Green)"
fi

# Red: no layout_generation / paint_generation in editor sources (TRACK prose OK).
if grep -R --include='*.mlc' -n -E 'layout_generation|paint_generation' "$EDITOR_DIR" >/dev/null 2>&1; then
  fail "editor sources already define layout_generation/paint_generation (expected until Green)"
fi

# Red: no type EditorFrame (EditorFrameInput / EditorFrameLayout predate this STEP).
if grep -R --include='*.mlc' -n -E 'type[[:space:]]+EditorFrame([^A-Za-z0-9_]|$)' "$EDITOR_DIR" >/dev/null 2>&1; then
  fail "editor sources already declare type EditorFrame (expected until Green)"
fi

# Red: ux_gate still does not wire ownership harness.
if grep -q 'run_editor_frame_ownership\|layout_generation\|paint_generation' "$UX_GATE"; then
  fail "run_ux_gate already wires frame ownership (Decision: not folded)"
fi

# Red: no §110b Green measured paste yet (do not match §110a ### Green measured).
if grep -A40 '### §110b Green measured\|Green measured (§110b)\|### Green measured (§110b)' "$TRACK" 2>/dev/null | \
  grep -Eq 'layout_generation=|paint_generation=|delta_layout_generation|delta_paint_generation'; then
  fail "TRACK already has ownership post-cut measured numbers (expected gap until Green)"
fi

fail "no editor frame ownership harness (Red expected)"
