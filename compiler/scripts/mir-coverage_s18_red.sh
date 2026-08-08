#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 18 — Red: no array flat_map
# HOF desugar. Green adds mir_lower_array_flat_map_hof_to_local + special-case
# in mir_lower_method_to_local (map-like + inner push/concat).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s18_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_array_flat_map_hof_to_local' "$LOWER_FN"; then
  fail "mir_lower_array_flat_map_hof_to_local already present (expected gap until Green)"
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

if printf '%s\n' "$METHOD_BODY" | grep -Eq "method_name == 'flat_map'|mir_lower_array_flat_map_hof"; then
  fail "method_to_local already special-cases flat_map (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 18)\|Green reopen measured (§104-6 slice 18)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|flat_map_hof|unsupported method flat_map'; then
  fail "TRACK already has §104-6 s18 post-cut measured counters (expected gap until Green)"
fi

fail "no flat_map HOF desugar (Red expected)"
