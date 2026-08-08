#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 22 — Red: no While/For
# to_local helpers and no operand|rvalue|expression_to_local While|For arms.
# (StatementExpr While/For already exist — those are not the gap.)
# Green adds to_local + operand/rvalue/expr arms.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s22_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_while_to_local|fn mir_lower_for_to_local' "$LOWER_FN"; then
  fail "While/For to_local helper already present (expected gap until Green)"
fi

extract_fn_body() {
  local name="$1"
  awk -v name="$name" '
    $0 ~ "^fn " name "\\(" {
      printing=1
      print
      next
    }
    printing {
      if ($0 ~ /^fn /) { exit }
      print
    }
  ' "$LOWER_FN"
}

for fn_name in \
  mir_lower_operand_from_expression \
  mir_lower_rvalue_from_expression \
  mir_lower_expression_to_local
do
  body="$(extract_fn_body "$fn_name" || true)"
  [ -n "$body" ] || fail "could not extract ${fn_name}"
  if printf '%s\n' "$body" | grep -Eq 'SemanticExpressionWhile|SemanticExpressionFor'; then
    fail "${fn_name} already arms While/For (expected gap until Green)"
  fi
done

# Drift check: statement-shaped While/For must still be present.
if ! grep -Eq 'SemanticExpressionWhile\(condition, body_statements' "$LOWER_FN"; then
  fail "unexpected: statement While arm missing (drift)"
fi
if ! grep -Eq 'SemanticExpressionFor\(variable_name, iterator, body_statements' "$LOWER_FN"; then
  fail "unexpected: statement For arm missing (drift)"
fi

if grep -A30 'Green measured (§104-6 slice 22)\|Green reopen measured (§104-6 slice 22)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|While→Unit|mir_lower_while_to_local'; then
  fail "TRACK already has §104-6 s22 post-cut measured counters (expected gap until Green)"
fi

fail "no While/For to_local / operand|rvalue|expr While|For arms (Red expected)"
