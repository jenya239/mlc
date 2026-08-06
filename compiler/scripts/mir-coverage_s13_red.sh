#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 13 — Red: no byte_size/upper/
# to_lower/take MIR natives. Green maps + implements __mir_string_byte_size /
# __mir_string_upper / __mir_string_to_lower / __mir_array_take.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s13_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"
[ -f "$RUNTIME" ] || fail "missing runtime.mlc (unexpected drift)"

WHITELIST="$(awk '/fn mir_lower_method_native_name/,/^fn |^export fn /' "$LOWER_FN" | head -n 50 || true)"
if printf '%s\n' "$WHITELIST" | grep -Eq "method_name == 'byte_size'|method_name == 'upper'|method_name == 'to_lower'|method_name == 'take'"; then
  fail "mir_lower_method_native_name already maps byte_size/upper/to_lower/take (expected gap until Green)"
fi

if grep -Eq '__mir_string_byte_size|__mir_string_upper|__mir_string_to_lower|__mir_array_take' "$NATIVE" "$RUNTIME"; then
  fail "byte_size/upper/to_lower/take natives already in VM (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 13)\|Green reopen measured (§104-6 slice 13)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|__mir_string_byte_size'; then
  fail "TRACK already has §104-6 s13 post-cut measured counters (expected gap until Green)"
fi

fail "no byte_size/upper/to_lower/take natives (Red expected)"
