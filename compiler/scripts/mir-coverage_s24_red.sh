#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 24 — Red: no Lambda
# value-to_local / no operand|rvalue Lambda arms; visit_int still unsupported.
# Green adds mir_lower_lambda_value_to_local + wiring + visit_int Unit mop-up.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s24_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_lambda_value_to_local' "$LOWER_FN"; then
  fail "lambda_value_to_local helper already present (expected gap until Green)"
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

for fn_name in mir_lower_operand_from_expression mir_lower_rvalue_from_expression; do
  body="$(extract_fn_body "$fn_name" || true)"
  [ -n "$body" ] || fail "could not extract ${fn_name}"
  if printf '%s\n' "$body" | grep -Eq 'SemanticExpressionLambda'; then
    fail "${fn_name} already arms Lambda (expected gap until Green)"
  fi
done

# expression_to_local may mention Lambda inside Call(function) — only forbid value helper use.
expr_body="$(extract_fn_body mir_lower_expression_to_local || true)"
[ -n "$expr_body" ] || fail "could not extract mir_lower_expression_to_local"
if printf '%s\n' "$expr_body" | grep -Eq 'mir_lower_lambda_value_to_local'; then
  fail "expression_to_local already routes Lambda via value helper (expected gap until Green)"
fi

native_body="$(extract_fn_body mir_lower_method_native_name || true)"
[ -n "$native_body" ] || fail "could not extract mir_lower_method_native_name"
if printf '%s\n' "$native_body" | grep -Eq "visit_int"; then
  fail "visit_int already special-cased in method_native_name (expected gap until Green)"
fi

if grep -Eq "method_name == .visit_int.|visit_int.*Unit|mir_lower_visit_int" "$LOWER_FN"; then
  fail "visit_int Unit mop-up already present (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 24)\|Green reopen measured (§104-6 slice 24)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|lambda_value|visit_int.*absent|LEC=0'; then
  fail "TRACK already has §104-6 s24 post-cut measured counters (expected gap until Green)"
fi

fail "no lambda_value_to_local / no operand|rvalue Lambda / visit_int unsupported (Red expected)"
