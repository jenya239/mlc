#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 10 — Red: no
# SemanticStatementLetPattern arm / mir_lower_let_pattern_statement.
# Green wires LetPattern via match bind helpers.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s10_red] FAIL: $1" >&2
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

if printf '%s\n' "$STATEMENT_BODY" | grep -Eq 'SemanticStatementLetPattern'; then
  fail "statement already has LetPattern arm (expected gap until Green)"
fi

if grep -Eq 'fn mir_lower_let_pattern_statement' "$LOWER_FN"; then
  fail "mir_lower_let_pattern_statement already present (expected gap until Green)"
fi

if grep -A40 'Green measured (§104-6 slice 10)\|### §104-6 slice 10 Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|LetPattern wired'; then
  fail "TRACK already has §104-6 s10 post-cut measured counters (expected gap until Green)"
fi

fail "no LetPattern statement arm/helper (Red expected)"
