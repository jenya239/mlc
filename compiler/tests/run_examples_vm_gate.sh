#!/usr/bin/env bash
# misc/examples --run subset gate (TRACK_MIR_VM_FULL Epic 4 STEP=12).
# Runs every program in vm_examples_run_programs.txt via mlcc --run.
# Compiler failures print `error:` on stdout and/or stderr — both checked.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
MANIFEST="${VM_EXAMPLES_RUN_MANIFEST:-$COMPILER_DIR/tests/vm_examples_run_programs.txt}"

if [ ! -x "$MLCC" ]; then
  echo "[examples vm gate] FAIL: mlcc not found at $MLCC" >&2
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
    echo "[examples vm gate] FAIL missing: $program" >&2
    fail_count=$((fail_count + 1))
    continue
  fi
  program_count=$((program_count + 1))
  out_file="$TMPDIR/mlcc_examples_run_$$.out"
  err_file="$TMPDIR/mlcc_examples_run_$$.err"
  set +e
  "$MLCC" --run "$program" >"$out_file" 2>"$err_file"
  status=$?
  set -e
  if grep -qE '^error:' "$out_file" "$err_file" 2>/dev/null; then
    echo "[examples vm gate] FAIL $program (compiler error, exit=$status)" >&2
    grep -hE '^error:' "$out_file" "$err_file" 2>/dev/null | head -3 >&2 || true
    fail_count=$((fail_count + 1))
  else
    echo "[examples vm gate] ok $program exit=$status" >&2
  fi
done <"$MANIFEST"
rm -f "$TMPDIR/mlcc_examples_run_$$.out" "$TMPDIR/mlcc_examples_run_$$.err"

if [ "$program_count" -eq 0 ]; then
  echo "[examples vm gate] FAIL: no programs in $MANIFEST" >&2
  exit 1
fi

if [ "$fail_count" -ne 0 ]; then
  echo "[examples vm gate] FAIL: $fail_count of $program_count" >&2
  exit 1
fi

echo "[examples vm gate] ok ($program_count programs)" >&2
