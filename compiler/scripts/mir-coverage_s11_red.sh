#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 11 — Red: no
# SemanticStatementBreak/Continue arms and no loop_break_targets stack.
# Green wires Break/Continue via loop target stack.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s11_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

extract_fn_body() {
  local name="$1"
  awk -v name="$name" '
    $0 ~ "^fn " name { printing=1 }
    printing {
      print
      if ($0 ~ /^fn / && $0 !~ "^fn " name) { exit }
    }
  ' "$LOWER_FN" | head -n -1
}

STATEMENT_BODY="$(extract_fn_body mir_lower_statement || true)"
[ -n "$STATEMENT_BODY" ] || fail "could not extract mir_lower_statement"

if printf '%s\n' "$STATEMENT_BODY" | grep -Eq 'SemanticStatementBreak|SemanticStatementContinue'; then
  fail "statement already has Break/Continue arms (expected gap until Green)"
fi

if grep -Eq 'loop_break_targets' "$LOWER_FN"; then
  fail "loop_break_targets already present (expected gap until Green)"
fi

if grep -A40 'Green measured (§104-6 slice 11)\|### §104-6 slice 11 Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|Break/Continue wired'; then
  fail "TRACK already has §104-6 s11 post-cut measured counters (expected gap until Green)"
fi

fail "no Break/Continue arms / loop_break_targets (Red expected)"
