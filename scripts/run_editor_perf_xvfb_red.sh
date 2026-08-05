#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110a — Red: no Xvfb isolate harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_xvfb.sh wrapping dogfood gate under Xvfb.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_xvfb.sh"
HELPER="$ROOT_DIR/scripts/editor_perf_maybe_xvfb.sh"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
UX_GATE="$ROOT_DIR/scripts/run_ux_gate.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"

fail() {
  echo "[editor_perf_xvfb_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$UX_GATE" ] || fail "missing run_ux_gate.sh (unexpected drift)"

# Red: green Xvfb wrapper absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_xvfb.sh already present (expected gap until Green)"
fi
if [ -f "$HELPER" ]; then
  fail "editor_perf_maybe_xvfb.sh already present (expected gap until Green)"
fi

# Red: no script yet consumes MLC_EDITOR_PERF_XVFB (TRACK prose alone OK).
if grep -R --include='*.sh' -l 'MLC_EDITOR_PERF_XVFB' "$ROOT_DIR/scripts" 2>/dev/null | \
  grep -v 'run_editor_perf_xvfb_red\.sh$' >/dev/null; then
  fail "scripts already read MLC_EDITOR_PERF_XVFB (expected until Green)"
fi

# Red: ux_gate still does not wire Xvfb / dogfood-under-xvfb.
if grep -q 'run_editor_perf_xvfb\|MLC_EDITOR_PERF_XVFB\|editor_perf_maybe_xvfb' "$UX_GATE"; then
  fail "run_ux_gate already wires xvfb wrapper (Decision: not folded)"
fi

# Red: no §110a Green measured paste yet.
if grep -A30 '### Green measured\|### §110a Green\|Green measured (§110a)' "$TRACK" 2>/dev/null | \
  grep -Eq 'scroll_cpu_percent=|child_exit=0|xvfb=1'; then
  fail "TRACK already has xvfb post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf xvfb harness (Red expected)"
