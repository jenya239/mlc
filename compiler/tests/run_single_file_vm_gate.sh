#!/usr/bin/env bash
# Single-file --run stability gate (TRACK_MIR_VM_FULL Epic 4 STEP=10).
# Runs every program in vm_cpp_diff_programs.txt via mlcc --run (exit 0 required
# only for programs that should succeed; non-zero exit codes are the program's
# return value and are accepted). Optionally runs VM/C++ exit-code parity.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
MANIFEST="${VM_CPP_DIFF_MANIFEST:-$COMPILER_DIR/tests/vm_cpp_diff_programs.txt}"
RUN_DIFF="${RUN_VM_CPP_DIFF:-1}"

if [ ! -x "$MLCC" ]; then
  echo "[single-file vm gate] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

TMPDIR="${TMPDIR:-/tmp}"
program_count=0
fail_count=0

while IFS= read -r line || [ -n "$line" ]; do
  case "$line" in
    ''|'#'*) continue ;;
    /*) program="$ROOT_DIR$line" ;;
    *) program="$ROOT_DIR/$line" ;;
  esac
  if [ ! -f "$program" ]; then
    echo "[single-file vm gate] FAIL missing: $program" >&2
    fail_count=$((fail_count + 1))
    continue
  fi
  program_count=$((program_count + 1))
  set +e
  "$MLCC" --run "$program" >/dev/null 2>"$TMPDIR/mlcc_single_file_$$.err"
  status=$?
  set -e
  # --run failure is diagnostic text on stderr and typically exit 1 with "error:"
  if grep -q '^error:' "$TMPDIR/mlcc_single_file_$$.err" 2>/dev/null; then
    echo "[single-file vm gate] FAIL $program (compiler error)" >&2
    head -3 "$TMPDIR/mlcc_single_file_$$.err" >&2 || true
    fail_count=$((fail_count + 1))
  else
    echo "[single-file vm gate] ok $program exit=$status" >&2
  fi
done <"$MANIFEST"
rm -f "$TMPDIR/mlcc_single_file_$$.err"

if [ "$program_count" -eq 0 ]; then
  echo "[single-file vm gate] FAIL: no programs in $MANIFEST" >&2
  exit 1
fi

if [ "$fail_count" -ne 0 ]; then
  echo "[single-file vm gate] FAIL: $fail_count of $program_count" >&2
  exit 1
fi

echo "[single-file vm gate] --run ok ($program_count programs)" >&2

if [ "$RUN_DIFF" = "1" ]; then
  MLCC="$MLCC" bash "$COMPILER_DIR/tests/run_vm_cpp_exit_diff.sh"
fi

echo "[single-file vm gate] ok" >&2
