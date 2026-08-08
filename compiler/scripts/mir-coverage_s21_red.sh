#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 21 — Red: no With/Extern
# operand arms, no Ident→Call HOF synthesize, no find_index HOF.
# Green adds those.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s21_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_with_to_local|fn mir_lower_extern_to_local' "$LOWER_FN"; then
  fail "With/Extern lower helper already present (expected gap until Green)"
fi

if grep -Eq 'SemanticExpressionWith|SemanticExpressionExtern' "$LOWER_FN"; then
  fail "SemanticExpressionWith/Extern already armed in lower_fn (expected gap until Green)"
fi

if grep -Eq 'fn mir_lower_array_find_index_hof_to_local|method_name == .find_index.' "$LOWER_FN"; then
  fail "find_index HOF desugar already present (expected gap until Green)"
fi

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

CALLBACK_BODY="$(extract_fn_body mir_lower_resolve_predicate_callback || true)"
[ -n "$CALLBACK_BODY" ] || fail "could not extract mir_lower_resolve_predicate_callback"

# Red: Ident path is lookup_lambda only — no synthesize / Call wrapper.
if printf '%s\n' "$CALLBACK_BODY" | grep -Eq 'synthesize|mir_lower_named_fn_callback|Call\(.*Ident|expected_arity|synthetic'; then
  fail "Ident HOF synthesize already present (expected gap until Green)"
fi

if ! printf '%s\n' "$CALLBACK_BODY" | grep -Eq "mir_lower_lookup_lambda"; then
  fail "unexpected: resolve_predicate_callback lost Ident→lookup_lambda (drift)"
fi

if grep -A30 'Green measured (§104-6 slice 21)\|Green reopen measured (§104-6 slice 21)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|unknown lambda|With/Extern|named-fn|find_index'; then
  fail "TRACK already has §104-6 s21 post-cut measured counters (expected gap until Green)"
fi

fail "no With/Extern operand arms / Ident HOF synthesize / find_index (Red expected)"
