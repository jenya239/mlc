#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109b — Red: no wake-on-hover gate / miss-geometry.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_wake_on_hover.sh + reason counters + text-rect probe
# (kills hardcoded dogfood_text_x = 420).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PERF="$ROOT_DIR/misc/editor/ui/perf.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_wake_on_hover_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$PERF" ] || fail "missing perf.mlc (unexpected drift)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Sanity: §109a dogfood + rebuild counters exist (retain baseline).
grep -q 'MLC_EDITOR_PERF_DOGFOOD' "$DEMO" || fail "demo_live missing dogfood probe (unexpected drift)"
grep -q 'content_rebuild_count' "$PERF" || fail "perf missing content_rebuild_count (unexpected drift)"
grep -q 'chrome_rebuild_count' "$PERF" || fail "perf missing chrome_rebuild_count (unexpected drift)"

# Red: green wake harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_wake_on_hover.sh already present (expected gap until Green)"
fi

# Red: frame-reason counters not wired yet.
if grep -q 'content_frame_count' "$PERF"; then
  fail "perf already has content_frame_count (expected gap until Green)"
fi
if grep -q 'idle_wait_count' "$PERF"; then
  fail "perf already has idle_wait_count (expected gap until Green)"
fi

# Red: wake probe env not wired.
if grep -q 'MLC_EDITOR_PERF_WAKE_PROBE' "$DEMO"; then
  fail "demo_live already has MLC_EDITOR_PERF_WAKE_PROBE (expected gap until Green)"
fi

# Red: still/jitter still use hardcoded miss-geometry (§109a 420).
if ! grep -q 'dogfood_text_x = 420' "$DEMO"; then
  fail "demo_live no longer has dogfood_text_x = 420 (expected miss-geometry until Green)"
fi

# Red: no wake measured table yet.
if grep -A5 '### Wake baseline (measured)' "$TRACK" | grep -Eq 'still_over_text_cpu_percent|[0-9]+%'; then
  fail "TRACK already has wake baseline numbers (expected gap until Green)"
fi

fail "no editor perf wake-on-hover gate (Red expected)"
