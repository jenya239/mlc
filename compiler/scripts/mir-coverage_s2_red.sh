#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 2 — Red: no lower_error_hist /
# substring|char_at MIR natives.
# Green adds hist lines + __mir_string_substring / __mir_string_char_at.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
REPORT="$ROOT_DIR/compiler/mir/mir_bootstrap_report.mlc"
COVERAGE="$ROOT_DIR/compiler/scripts/mir-coverage.sh"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s2_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$REPORT" ] || fail "missing mir_bootstrap_report.mlc (unexpected drift)"
[ -f "$COVERAGE" ] || fail "missing mir-coverage.sh (s1 prerequisite)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"
[ -f "$RUNTIME" ] || fail "missing runtime.mlc (unexpected drift)"

# Red: histogram absent until Green.
if grep -Eq 'lower_error_hist' "$REPORT" "$COVERAGE"; then
  fail "lower_error_hist already present (expected gap until Green)"
fi

# Red: method whitelist has no substring/char_at until Green.
WHITELIST="$(awk '/fn mir_lower_method_native_name/,/^fn |^export fn /' "$LOWER_FN" | head -n 20 || true)"
if printf '%s\n' "$WHITELIST" | grep -Eq "method_name == 'substring'|method_name == 'char_at'"; then
  fail "mir_lower_method_native_name already maps substring/char_at (expected gap until Green)"
fi

# Red: VM natives absent until Green.
if grep -Eq '__mir_string_substring|__mir_string_char_at' "$NATIVE" "$RUNTIME"; then
  fail "__mir_string_substring/char_at already in VM (expected gap until Green)"
fi

# Red: no §104-6 s2 Green measured paste yet.
if grep -A30 'Green measured (§104-6 slice 2)\|### §104-6 slice 2 Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_hist:|lower_error_count=[0-9]+'; then
  fail "TRACK already has §104-6 s2 post-cut measured counters (expected gap until Green)"
fi

fail "no lower_error_hist / substring|char_at natives (Red expected)"
