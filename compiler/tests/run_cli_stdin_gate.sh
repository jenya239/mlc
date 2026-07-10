#!/usr/bin/env bash
# TRACK_CLI_STDIN: `mlcc --run -` and `/dev/stdin` must not crash; `-` reads program from stdin.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
TIMEOUT_SEC="${CLI_STDIN_TIMEOUT_SEC:-10}"
TMPDIR="${TMPDIR:-/tmp}"

if [ ! -x "$MLCC" ]; then
  echo "[cli stdin] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

fail_count=0
case_count=0

run_piped() {
  local label="$1"
  local expected_exit="$2"
  local entry_path="$3"
  local source="$4"
  local out_file err_file status
  out_file="${TMPDIR}/mlcc_cli_stdin_$$_${label}.out"
  err_file="${TMPDIR}/mlcc_cli_stdin_$$_${label}.err"
  set +e
  printf '%s' "$source" | timeout "$TIMEOUT_SEC" "$MLCC" --run "$entry_path" >"$out_file" 2>"$err_file"
  status=$?
  set -e
  if [ "$status" -eq 124 ]; then
    echo "[cli stdin] FAIL $label hung (timeout ${TIMEOUT_SEC}s)" >&2
    fail_count=$((fail_count + 1))
    rm -f "$out_file" "$err_file"
    return 0
  fi
  if grep -qiE 'length_error|Aborted|SIGABRT|terminate called' "$out_file" "$err_file" 2>/dev/null; then
    echo "[cli stdin] FAIL $label crash/length_error exit=$status" >&2
    head -5 "$err_file" >&2 || true
    fail_count=$((fail_count + 1))
    rm -f "$out_file" "$err_file"
    return 0
  fi
  if grep -qE '^error:' "$out_file" "$err_file" 2>/dev/null; then
    echo "[cli stdin] FAIL $label compiler error exit=$status" >&2
    grep -hE '^error:' "$out_file" "$err_file" 2>/dev/null | head -3 >&2 || true
    fail_count=$((fail_count + 1))
    rm -f "$out_file" "$err_file"
    return 0
  fi
  if [ "$status" -ne "$expected_exit" ]; then
    echo "[cli stdin] FAIL $label exit=$status expected=$expected_exit" >&2
    fail_count=$((fail_count + 1))
    rm -f "$out_file" "$err_file"
    return 0
  fi
  rm -f "$out_file" "$err_file"
  echo "[cli stdin] ok $label exit=$status" >&2
}

SOURCE_SEVEN='fn main() -> i32 = 7
'
SOURCE_ELEVEN='fn main() -> i32 = 11
'

run_piped "dash" 7 "-" "$SOURCE_SEVEN"
case_count=$((case_count + 1))
run_piped "dev_stdin" 11 "/dev/stdin" "$SOURCE_ELEVEN"
case_count=$((case_count + 1))

if [ "$fail_count" -ne 0 ]; then
  echo "[cli stdin] FAIL ($fail_count/$case_count)" >&2
  exit 1
fi
echo "[cli stdin] ok ($case_count cases)" >&2
