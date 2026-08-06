#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 5 — Red: no Block/Char/Float
# arms in mir_lower_operand_from_expression / mir_lower_rvalue_from_expression.
# Green adds those arms (Block via into-local; Char/Float as ConstStr).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s5_red] FAIL: $1" >&2
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

OPERAND_BODY="$(extract_fn_body mir_lower_operand_from_expression || true)"
RVALUE_BODY="$(extract_fn_body mir_lower_rvalue_from_expression || true)"

[ -n "$OPERAND_BODY" ] || fail "could not extract mir_lower_operand_from_expression"
[ -n "$RVALUE_BODY" ] || fail "could not extract mir_lower_rvalue_from_expression"

if printf '%s\n' "$OPERAND_BODY" | grep -Eq 'SemanticExpressionBlock|SemanticExpressionChar|SemanticExpressionFloat'; then
  fail "operand already has Block/Char/Float arms (expected gap until Green)"
fi

if printf '%s\n' "$RVALUE_BODY" | grep -Eq 'SemanticExpressionBlock|SemanticExpressionChar|SemanticExpressionFloat'; then
  fail "rvalue already has Block/Char/Float arms (expected gap until Green)"
fi

if grep -A40 'Green measured (§104-6 slice 5)\|### §104-6 slice 5 Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|operand hist'; then
  fail "TRACK already has §104-6 s5 post-cut measured counters (expected gap until Green)"
fi

fail "no Block/Char/Float in operand|rvalue (Red expected)"
