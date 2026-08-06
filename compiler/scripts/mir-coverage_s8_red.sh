#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 8 — Red: no byte_substring MIR native.
# Green adds __mir_string_byte_substring + whitelist map.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s8_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"
[ -f "$RUNTIME" ] || fail "missing runtime.mlc (unexpected drift)"

WHITELIST="$(awk '/fn mir_lower_method_native_name/,/^fn |^export fn /' "$LOWER_FN" | head -n 40 || true)"
if printf '%s\n' "$WHITELIST" | grep -Eq "method_name == 'byte_substring'"; then
  fail "mir_lower_method_native_name already maps byte_substring (expected gap until Green)"
fi

if grep -Eq '__mir_string_byte_substring' "$NATIVE" "$RUNTIME"; then
  fail "__mir_string_byte_substring already in VM (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 8)\|### §104-6 slice 8 Green' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|__mir_string_byte_substring'; then
  fail "TRACK already has §104-6 s8 post-cut measured counters (expected gap until Green)"
fi

fail "no byte_substring native (Red expected)"
