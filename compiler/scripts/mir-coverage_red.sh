#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 1 — Red: no mir-coverage /
# lower_error_count in bootstrap report.
# Green adds compiler/scripts/mir-coverage.sh + lower_error_count= in
# mir_bootstrap_report printer.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COVERAGE="$ROOT_DIR/compiler/scripts/mir-coverage.sh"
REPORT="$ROOT_DIR/compiler/mir/mir_bootstrap_report.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"
BOOTSTRAP_SH="$ROOT_DIR/compiler/scripts/mir_bootstrap_report.sh"

fail() {
  echo "[mir-coverage_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_COMPILER_ARCHITECTURE_HYGIENE.md (unexpected drift)"
[ -f "$REPORT" ] || fail "missing mir_bootstrap_report.mlc (unexpected drift)"
[ -f "$BOOTSTRAP_SH" ] || fail "missing mir_bootstrap_report.sh (unexpected drift)"

# Red: coverage script absent until Green.
if [ -f "$COVERAGE" ]; then
  fail "mir-coverage.sh already present (expected gap until Green)"
fi

# Red: report printer has no lower_error_count until Green.
if grep -Eq 'lower_error_count' "$REPORT"; then
  fail "mir_bootstrap_report already declares lower_error_count (expected gap until Green)"
fi

# Red: no §104-6 s1 Green measured paste yet.
if grep -A40 '### §104-6 slice 1 Green measured\|Green measured (§104-6 slice 1)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=|mir-coverage=ok'; then
  fail "TRACK already has §104-6 s1 post-cut measured counters (expected gap until Green)"
fi

fail "no mir-coverage / lower_error_count (Red expected)"
