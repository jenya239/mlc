#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 17 — Red: no array fold
# HOF desugar. Green adds mir_lower_array_fold_hof_to_local + special-case
# in mir_lower_method_to_local (init + 2-arg callback).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s17_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_array_fold_hof_to_local' "$LOWER_FN"; then
  fail "mir_lower_array_fold_hof_to_local already present (expected gap until Green)"
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

METHOD_BODY="$(extract_fn_body mir_lower_method_to_local || true)"
[ -n "$METHOD_BODY" ] || fail "could not extract mir_lower_method_to_local"

if printf '%s\n' "$METHOD_BODY" | grep -Eq "method_name == 'fold'|mir_lower_array_fold_hof"; then
  fail "method_to_local already special-cases fold (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 17)\|Green reopen measured (§104-6 slice 17)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|fold_hof|unsupported method fold'; then
  fail "TRACK already has §104-6 s17 post-cut measured counters (expected gap until Green)"
fi

fail "no fold HOF desugar (Red expected)"
