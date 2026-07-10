#!/usr/bin/env bash
# TRACK_VM_BLOCK_ID_COLLISION STEP=2: elif/nested-if must not hang or mis-branch.
# Pre-fix: classify(1) infinite-looped (duplicate MIR block id).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
TIMEOUT_SEC="${VM_ELIF_TIMEOUT_SEC:-5}"

if [ ! -x "$MLCC" ]; then
  echo "[vm elif block-id] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

run_case() {
  local program="$1"
  local expected_exit="$2"
  local label="$3"
  if [ ! -f "$program" ]; then
    echo "[vm elif block-id] FAIL missing: $program" >&2
    return 1
  fi
  local out_file err_file status
  out_file="${TMPDIR:-/tmp}/mlcc_elif_$$_${label}.out"
  err_file="${TMPDIR:-/tmp}/mlcc_elif_$$_${label}.err"
  set +e
  timeout "$TIMEOUT_SEC" "$MLCC" --run "$program" >"$out_file" 2>"$err_file"
  status=$?
  set -e
  if [ "$status" -eq 124 ]; then
    echo "[vm elif block-id] FAIL $label hung (timeout ${TIMEOUT_SEC}s): $program" >&2
    rm -f "$out_file" "$err_file"
    return 1
  fi
  if grep -qE '^error:' "$out_file" "$err_file" 2>/dev/null; then
    echo "[vm elif block-id] FAIL $label compiler error exit=$status: $program" >&2
    grep -hE '^error:' "$out_file" "$err_file" 2>/dev/null | head -3 >&2 || true
    rm -f "$out_file" "$err_file"
    return 1
  fi
  if [ "$status" -ne "$expected_exit" ]; then
    echo "[vm elif block-id] FAIL $label exit=$status expected=$expected_exit: $program" >&2
    rm -f "$out_file" "$err_file"
    return 1
  fi
  rm -f "$out_file" "$err_file"
  echo "[vm elif block-id] ok $label exit=$status" >&2
  return 0
}

fail_count=0
run_case "$ROOT_DIR/misc/examples/vm_elif_classify_0.mlc" 100 "classify0" || fail_count=$((fail_count + 1))
run_case "$ROOT_DIR/misc/examples/vm_elif_classify_1.mlc" 200 "classify1" || fail_count=$((fail_count + 1))
# 300 % 256 == 44 (mlcc --run exit is result mod 256)
run_case "$ROOT_DIR/misc/examples/vm_elif_classify_5.mlc" 44 "classify5" || fail_count=$((fail_count + 1))

if [ "$fail_count" -ne 0 ]; then
  echo "[vm elif block-id] FAIL: $fail_count of 3" >&2
  exit 1
fi
echo "[vm elif block-id] ok (3 cases)" >&2
