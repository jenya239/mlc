#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 4 — Red: no concat|has
# MIR natives.
# Green adds __mir_array_concat / __mir_map_has + whitelist maps.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s4_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"
[ -f "$RUNTIME" ] || fail "missing runtime.mlc (unexpected drift)"

WHITELIST="$(awk '/fn mir_lower_method_native_name/,/^fn |^export fn /' "$LOWER_FN" | head -n 40 || true)"
if printf '%s\n' "$WHITELIST" | grep -Eq "method_name == 'concat'|method_name == 'has'"; then
  fail "mir_lower_method_native_name already maps concat/has (expected gap until Green)"
fi

if grep -Eq '__mir_array_concat|__mir_map_has' "$NATIVE" "$RUNTIME"; then
  fail "__mir_array_concat/__mir_map_has already in VM (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 4)\|### §104-6 slice 4 Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|__mir_array_concat'; then
  fail "TRACK already has §104-6 s4 post-cut measured counters (expected gap until Green)"
fi

fail "no concat|has natives (Red expected)"
