#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 12 — Red: no index_of/trim/drop
# MIR natives. Green maps + implements __mir_string_index_of / __mir_string_trim /
# __mir_array_drop.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s12_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"
[ -f "$RUNTIME" ] || fail "missing runtime.mlc (unexpected drift)"

WHITELIST="$(awk '/fn mir_lower_method_native_name/,/^fn |^export fn /' "$LOWER_FN" | head -n 50 || true)"
if printf '%s\n' "$WHITELIST" | grep -Eq "method_name == 'index_of'|method_name == 'trim'|method_name == 'drop'"; then
  fail "mir_lower_method_native_name already maps index_of/trim/drop (expected gap until Green)"
fi

if grep -Eq '__mir_string_index_of|__mir_string_trim|__mir_array_drop' "$NATIVE" "$RUNTIME"; then
  fail "index_of/trim/drop natives already in VM (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 12)\|Green reopen measured (§104-6 slice 12)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|__mir_string_index_of'; then
  fail "TRACK already has §104-6 s12 post-cut measured counters (expected gap until Green)"
fi

fail "no index_of/trim/drop natives (Red expected)"
