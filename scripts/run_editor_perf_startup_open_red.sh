#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109j — Red: no startup-open harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_perf_startup_open.sh + MLC_EDITOR_OPEN resolve +
# first-present-before-full-wrap + report time_to_first_present_ms.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_perf_startup_open.sh"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
DOGFOOD="$ROOT_DIR/scripts/run_editor_perf_dogfood_baseline.sh"
MINIMAP="$ROOT_DIR/scripts/run_editor_perf_minimap_sample.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"

fail() {
  echo "[editor_perf_startup_open_red] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing demo_live.mlc (unexpected drift)"
[ -f "$DOGFOOD" ] || fail "missing dogfood baseline (unexpected drift — §109a required)"
[ -f "$MINIMAP" ] || fail "missing minimap-sample harness (unexpected drift — §109i required)"
[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_PERF_DOGFOOD.md (unexpected drift)"

# Red: green startup-open harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_perf_startup_open.sh already present (expected gap until Green)"
fi

# Red: interactive MLC_EDITOR_OPEN not wired yet (§109j Green).
if grep -q 'MLC_EDITOR_OPEN' "$DEMO"; then
  fail "MLC_EDITOR_OPEN already present in demo_live (expected gap until Green)"
fi

# Red: no resolve_startup_tabs / equivalent helper name yet.
if grep -qE 'resolve_startup_tabs|editor_ux_resolve_startup' "$DEMO"; then
  fail "startup resolve helper already present (expected gap until Green)"
fi

# Red: Decision pre-cut — open_disk_starter still README-first.
if ! awk '
  /fn open_disk_starter\(/ { in_fn=1 }
  in_fn && /try_open_path\(tabs, root \+ "\/README\.md"\)/ { readme=1 }
  in_fn && /try_open_path\(tabs, root \+ "\/misc\/editor\/demo_live\.mlc"\)/ {
    if (!readme) { early_demo=1 }
  }
  in_fn && /^fn / && !/fn open_disk_starter\(/ { in_fn=0 }
  END { exit((readme && !early_demo) ? 0 : 1) }
' "$DEMO"; then
  fail "open_disk_starter no longer README-before-demo_live (expected until Green)"
fi

# Red: no first-present / time_to_first_present markers yet.
if grep -qE 'time_to_first_present_ms|startup_full_wrap_deferred' "$DEMO"; then
  fail "startup present markers already present (expected gap until Green)"
fi

# Red: no post-cut startup-open measured numbers yet.
if grep -A30 '### Startup.open\|### Startup-open Green\|### §109j Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'time_to_first_present_ms \| [0-9]+|open_path \| .*demo_live'; then
  fail "TRACK already has startup-open post-cut measured numbers (expected gap until Green)"
fi

fail "no editor perf startup-open harness (Red expected)"
