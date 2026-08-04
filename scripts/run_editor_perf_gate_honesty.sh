#!/usr/bin/env bash
# TRACK_EDITOR_PERF_DOGFOOD §109c — gate honesty (hover L2 + PERF_FULL ceiling).
# Load-bearing hover CPU = wake harness; this script proves false-green paths gone.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
WAKE="$ROOT_DIR/scripts/run_editor_perf_wake_on_hover.sh"
HOVER="$ROOT_DIR/scripts/run_ux_hover_cpu_budget.sh"
PERF_FULL="$ROOT_DIR/scripts/run_editor_demo_live_perf_full_smoke.sh"
PERF_SMOKE="$ROOT_DIR/scripts/run_editor_demo_live_perf_smoke.sh"
IDLE="$ROOT_DIR/scripts/run_ux_idle_cpu_budget_stable.sh"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_PERF_DOGFOOD.md"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
REPORT_DIR="${EDITOR_PERF_HONESTY_OUT:-$ROOT_DIR/.tmp/editor_perf_gate_honesty}"
REPORT_FILE="$REPORT_DIR/report.txt"

fail() {
  echo "[editor_perf_gate_honesty] FAIL: $1" >&2
  exit 1
}

[ -f "$WAKE" ] || fail "missing wake harness (hover authority)"
[ -f "$HOVER" ] || fail "missing hover_cpu_budget"
[ -f "$PERF_FULL" ] || fail "missing PERF_FULL smoke"
[ -f "$TRACK" ] || fail "missing TRACK"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"

grep -q 'MLC_GLFW_VISIBLE=1' "$WAKE" || fail "wake missing VISIBLE=1"
grep -q 'MLC_EDITOR_PERF_OPEN' "$WAKE" || fail "wake missing PERF_OPEN"

# (2) hover L2 rewritten — no VISIBLE=0 / cpu=0 ceiling-10 path.
if grep -q 'export MLC_GLFW_VISIBLE=0' "$HOVER"; then
  fail "hover_cpu_budget still sets VISIBLE=0 (false-green path)"
fi
grep -q 'export MLC_GLFW_VISIBLE=1' "$HOVER" || fail "hover_cpu_budget L2 missing VISIBLE=1"
grep -q 'MLC_EDITOR_PERF_OPEN' "$HOVER" || fail "hover_cpu_budget L2 missing PERF_OPEN"
if grep -q 'COMMITTED_HOVER_CPU_BUDGET_PERCENT=10' "$HOVER"; then
  fail "hover still has COMMITTED_HOVER_CPU_BUDGET_PERCENT=10"
fi
grep -q 'COMMITTED_HOVER_CPU_BUDGET_PERCENT=8' "$HOVER" || fail "hover missing committed ceiling 8"
if grep -q 'cpu_percent=0' "$HOVER"; then
  fail "hover still has cpu=0 provenance comment"
fi
grep -q 'not sufficient\|not sole epic hover\|wake authority' "$HOVER" || \
  fail "hover header missing wake-authority disclaimer"

# Idle disclaimer.
[ -f "$IDLE" ] || fail "missing idle gate"
grep -qi 'must not be cited as still-over-text' "$IDLE" || \
  fail "idle gate missing still-over-text disclaimer"

# PERF skip-heavy must not be the content-budget entry (FULL is).
if [ -f "$PERF_SMOKE" ]; then
  if grep -q 'MLC_EDITOR_PERF_FULL=1' "$PERF_SMOKE"; then
    fail "baseline perf smoke unexpectedly sets PERF_FULL"
  fi
  grep -q 'MLC_EDITOR_PERF=1\|MLC_EDITOR_PERF=' "$PERF_SMOKE" || true
fi

# (3) PERF_FULL default TOTAL_US_MAX < 20000000 and ≤ measured×1.25 (TRACK).
# Prefer §109d Content-frame Green table when present (current ceiling authority);
# else §109c Gate honesty table. Do not let an older TOTAL_US_MAX row shadow the script.
default_max="$(
  grep -E 'TOTAL_US_MAX=.*:-' "$PERF_FULL" | head -1 | sed -n 's/.*:-\([0-9][0-9]*\)}.*/\1/p'
)"
[ -n "$default_max" ] || fail "cannot parse PERF_FULL TOTAL_US_MAX default"
if [ "$default_max" -ge 20000000 ]; then
  fail "PERF_FULL TOTAL_US_MAX default=$default_max still ≥ 20000000"
fi

# Ceiling authority = latest rewrite of PERF_FULL default: §109e Glyph-layer,
# else §109d Content-frame, else §109c Gate honesty (historical).
ceiling_line="$(
  awk '
    /^### Glyph-layer Green/ { in_section=1 }
    /^### / && !/^### Glyph-layer Green/ { in_section=0 }
    in_section && /TOTAL_US_MAX/ { print; exit }
  ' "$TRACK" || true
)"
measured_line="$(
  awk '
    /^### Glyph-layer Green/ { in_section=1 }
    /^### / && !/^### Glyph-layer Green/ { in_section=0 }
    in_section && /PERF_FULL .total_us./ { print; exit }
  ' "$TRACK" || true
)"
if [ -z "$ceiling_line" ]; then
  measured_line="$(grep -E '^\| measured_total_us \(ceiling basis\) \|' "$TRACK" | head -1 || true)"
  ceiling_line="$(
    awk '
      /^### Content-frame Green/ { in_section=1 }
      /^### / && !/^### Content-frame Green/ { in_section=0 }
      in_section && /^\| TOTAL_US_MAX \|/ { print; exit }
    ' "$TRACK" || true
  )"
fi
if [ -z "$ceiling_line" ]; then
  measured_line="$(grep -E '^\| measured_total_us \|' "$TRACK" | head -1 || true)"
  ceiling_line="$(
    awk '
      /^### Gate honesty/ { in_section=1 }
      /^### / && !/^### Gate honesty/ { in_section=0 }
      in_section && /^\| TOTAL_US_MAX \|/ { print; exit }
    ' "$TRACK" || true
  )"
fi
measured_us="$(printf '%s\n' "$measured_line" | grep -Eo '[0-9]{6,}' | head -1 || true)"
track_ceiling="$(printf '%s\n' "$ceiling_line" | grep -Eo '[0-9]{6,}' | head -1 || true)"
if [ -z "$measured_us" ] && [ -n "$track_ceiling" ]; then
  measured_us=$(( track_ceiling * 100 / 125 ))
fi
[ -n "$measured_us" ] || fail "TRACK missing measured_total_us for current PERF_FULL ceiling"
[ -n "$track_ceiling" ] || fail "TRACK missing TOTAL_US_MAX for current PERF_FULL ceiling"
if [ "$track_ceiling" -ne "$default_max" ]; then
  fail "TRACK TOTAL_US_MAX=$track_ceiling != script default $default_max"
fi
max_allowed=$(( measured_us * 125 / 100 ))
if [ "$default_max" -gt "$max_allowed" ]; then
  fail "PERF_FULL default $default_max > measured×1.25 ($max_allowed)"
fi

mkdir -p "$REPORT_DIR"

# (4) wake L2 under VISIBLE=1
export EDITOR_PERF_WAKE_OUT="${EDITOR_PERF_WAKE_OUT:-$REPORT_DIR/wake}"
export WAKE_SAMPLE_SEC="${WAKE_SAMPLE_SEC:-2}"
export WAKE_SAMPLE_ROUNDS="${WAKE_SAMPLE_ROUNDS:-2}"
export WAKE_SETTLE_SEC="${WAKE_SETTLE_SEC:-2}"
export WAKE_WARMUP_SEC="${WAKE_WARMUP_SEC:-3}"
bash "$WAKE"
wake_report="$EDITOR_PERF_WAKE_OUT/report.txt"
[ -f "$wake_report" ] || fail "missing wake report"
still="$(grep -E '^still_over_text_cpu_percent=' "$wake_report" | cut -d= -f2)"
jitter="$(grep -E '^text_jitter_cpu_percent=' "$wake_report" | cut -d= -f2)"
[ -n "$still" ] && [ -n "$jitter" ] || fail "wake report missing still/jitter"

# (5) PERF_FULL under new ceiling
export EDITOR_DEMO_LIVE_PERF_FULL_OUT="${EDITOR_DEMO_LIVE_PERF_FULL_OUT:-$REPORT_DIR/perf_full_compile}"
set +e
perf_out="$(bash "$PERF_FULL" 2>&1)"
perf_status=$?
set -e
printf '%s\n' "$perf_out"
[ "$perf_status" -eq 0 ] || fail "PERF_FULL smoke exit=$perf_status"
total_line="$(printf '%s\n' "$perf_out" | grep -E 'demo_live_perf_full frames=' | tail -1 || true)"
[ -n "$total_line" ] || fail "missing demo_live_perf_full line"
total_us="$(printf '%s\n' "$total_line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
[ -n "$total_us" ] || fail "cannot parse total_us"
if [ "$total_us" -gt "$default_max" ]; then
  fail "remeasure total_us=$total_us > TOTAL_US_MAX=$default_max"
fi

{
  echo "still_over_text_cpu_percent=$still"
  echo "text_jitter_cpu_percent=$jitter"
  echo "measured_total_us=$measured_us"
  echo "TOTAL_US_MAX=$default_max"
  echo "remeasure_total_us=$total_us"
} >"$REPORT_FILE"

echo "editor_perf_gate_honesty_ok"
echo "[editor_perf_gate_honesty] still=${still}% jitter=${jitter}% PERF_FULL total_us=${total_us} ceiling=${default_max}" >&2
cat "$REPORT_FILE"
