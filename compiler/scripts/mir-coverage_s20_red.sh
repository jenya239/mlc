#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 20 — Red: no
# __mir_record_with_field, no RecordUpdate lower arms, no Field-receiver
# mutating write-back. Green adds those.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s20_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"
[ -f "$RUNTIME" ] || fail "missing runtime.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_record_update_to_local' "$LOWER_FN"; then
  fail "mir_lower_record_update_to_local already present (expected gap until Green)"
fi

if grep -Eq "__mir_record_with_field" "$LOWER_FN" "$NATIVE" "$RUNTIME" 2>/dev/null; then
  fail "__mir_record_with_field already present (expected gap until Green)"
fi

if grep -Eq "__mir_file_make_temp_directory" "$LOWER_FN" "$NATIVE" "$RUNTIME" 2>/dev/null; then
  fail "__mir_file_make_temp_directory already present (expected gap until Green)"
fi

if grep -Eq 'SemanticExpressionRecordUpdate' "$LOWER_FN"; then
  fail "SemanticExpressionRecordUpdate already armed in lower_fn (expected gap until Green)"
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

# Mutating Expr stmt: only Ident receiver today; Field write-back must be absent.
if grep -Eq 'field_receiver_mutating|mir_lower_field_mutating' "$LOWER_FN"; then
  fail "Field-receiver mutating write-back path already present (expected gap until Green)"
fi

STATEMENT_BODY="$(extract_fn_body mir_lower_statement || true)"
[ -n "$STATEMENT_BODY" ] || fail "could not extract mir_lower_statement"

if printf '%s\n' "$STATEMENT_BODY" | grep -Eq "SemanticExpressionField|__mir_record_with_field|mir_lower_record_update"; then
  fail "statement lower already has Field mutate / RecordUpdate path (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 20)\|Green reopen measured (§104-6 slice 20)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|record_with_field|RecordUpdate wired|Field mutate'; then
  fail "TRACK already has §104-6 s20 post-cut measured counters (expected gap until Green)"
fi

fail "no record_with_field / RecordUpdate arms / Field mutate path (Red expected)"
