#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 23 — Red: unknown Ident
# still Err(unknown identifier); no funref ConstStr stub helper.
# Green adds mir_lower_funref_ident_to_local + wires Ident failure paths.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s23_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_funref_ident_to_local' "$LOWER_FN"; then
  fail "funref Ident helper already present (expected gap until Green)"
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

# Each of the three Ident lower paths must still Err on unknown non-ctor idents.
for fn_name in \
  mir_lower_operand_from_expression \
  mir_lower_rvalue_from_expression \
  mir_lower_expression_to_local
do
  body="$(extract_fn_body "$fn_name" || true)"
  [ -n "$body" ] || fail "could not extract ${fn_name}"
  if ! printf '%s\n' "$body" | grep -Fq 'unknown identifier'; then
    fail "${fn_name} lost unknown-identifier Err (drift / premature Green)"
  fi
  if printf '%s\n' "$body" | grep -Eq 'mir_lower_funref_ident_to_local|funref'; then
    fail "${fn_name} already routes unknown Ident via funref stub (expected gap until Green)"
  fi
done

if grep -A30 'Green measured (§104-6 slice 23)\|Green reopen measured (§104-6 slice 23)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|funref|ConstStr stub|unknown identifier.*absent'; then
  fail "TRACK already has §104-6 s23 post-cut measured counters (expected gap until Green)"
fi

fail "no funref Ident stub / Ident still Err(unknown identifier) (Red expected)"
