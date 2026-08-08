#!/usr/bin/env bash
# TRACK_COMPILER_ARCHITECTURE_HYGIENE §104-6 slice 19 — Red: no module-qualified
# MethodCall→Call lowering and no File.exists/read/write MIR natives.
# Green adds static/module path in mir_lower_method_to_local + __mir_file_*.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
LOWER_FN="$ROOT_DIR/compiler/mir/lower_fn.mlc"
NATIVE="$ROOT_DIR/compiler/vm/native.mlc"
RUNTIME="$ROOT_DIR/compiler/vm/runtime.mlc"
TRACK="$ROOT_DIR/docs/agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md"

fail() {
  echo "[mir-coverage_s19_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK (unexpected drift)"
[ -f "$LOWER_FN" ] || fail "missing lower_fn.mlc (unexpected drift)"
[ -f "$NATIVE" ] || fail "missing native.mlc (unexpected drift)"

if grep -Eq 'fn mir_lower_module_qualified_method_to_local|fn mir_lower_static_module_call_to_local' "$LOWER_FN"; then
  fail "module-qualified lower helper already present (expected gap until Green)"
fi

if grep -Eq "__mir_file_exists|__mir_file_read|__mir_file_write" "$LOWER_FN" "$NATIVE" "$RUNTIME" 2>/dev/null; then
  fail "__mir_file_* already present (expected gap until Green)"
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

if printf '%s\n' "$METHOD_BODY" | grep -Eq "receiver_name == 'File'|mir_lower_lookup_local|module_qualified|static_module"; then
  fail "method_to_local already has module/File static path (expected gap until Green)"
fi

NATIVE_NAME_BODY="$(extract_fn_body mir_lower_method_native_name || true)"
[ -n "$NATIVE_NAME_BODY" ] || fail "could not extract mir_lower_method_native_name"

if printf '%s\n' "$NATIVE_NAME_BODY" | grep -Eq "method_name == 'exists'|method_name == 'read'|method_name == 'write'|__mir_file_"; then
  fail "method_native_name already maps File exists/read/write (expected gap until Green)"
fi

if grep -A30 'Green measured (§104-6 slice 19)\|Green reopen measured (§104-6 slice 19)' "$TRACK" 2>/dev/null | \
  grep -Eq 'lower_error_count=[0-9]+|module_qualified|__mir_file_exists|make_identifier_cpp_expression'; then
  fail "TRACK already has §104-6 s19 post-cut measured counters (expected gap until Green)"
fi

fail "no module-qualified call / File natives (Red expected)"
