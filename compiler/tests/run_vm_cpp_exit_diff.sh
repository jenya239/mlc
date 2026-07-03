#!/usr/bin/env bash
# VM vs C++ exit-code differential harness (TRACK_MIR_VM STEP=7).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
MANIFEST="${VM_CPP_DIFF_MANIFEST:-$COMPILER_DIR/tests/vm_cpp_diff_programs.txt}"

if [ ! -x "$MLCC" ]; then
  echo "[vm cpp exit diff] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

TMPDIR="${TMPDIR:-/tmp}"
WORK=$(mktemp -d "$TMPDIR/mlcc_vm_cpp_diff_XXXXXX")
trap 'rm -rf "$WORK"' EXIT

run_one() {
  local program=$1
  local base
  base=$(basename "$program" .mlc)
  local build_dir="$WORK/$base"
  mkdir -p "$build_dir"

  set +e
  "$MLCC" --run "$program"
  local vm_exit=$?
  set -e

  if ! "$MLCC" -o "$build_dir" "$program" >/dev/null 2>"$build_dir/compile.err"; then
    echo "[vm cpp exit diff] FAIL $program: C++ emit failed" >&2
    head -3 "$build_dir/compile.err" >&2 || true
    return 1
  fi

  if ! MLCC_ENTRY_BASENAME="$base" "$COMPILER_DIR/build_bin.sh" "$build_dir" "$build_dir/program" 2>"$build_dir/link.err"; then
    echo "[vm cpp exit diff] FAIL $program: link failed" >&2
    head -3 "$build_dir/link.err" >&2 || true
    return 1
  fi

  set +e
  "$build_dir/program" >/dev/null
  local cpp_exit=$?
  set -e

  if [ "$vm_exit" -ne "$cpp_exit" ]; then
    echo "[vm cpp exit diff] FAIL $program: vm=$vm_exit cpp=$cpp_exit" >&2
    return 1
  fi
  echo "[vm cpp exit diff] ok $program exit=$vm_exit" >&2
}

load_programs() {
  if [ "$#" -gt 0 ]; then
    printf '%s\n' "$@"
    return
  fi
  if [ ! -f "$MANIFEST" ]; then
    echo "[vm cpp exit diff] FAIL: manifest missing: $MANIFEST" >&2
    exit 1
  fi
  while IFS= read -r line || [ -n "$line" ]; do
    case "$line" in
      ''|'#'*) continue ;;
      /*) program="$ROOT_DIR$line" ;;
      *) program="$ROOT_DIR/$line" ;;
    esac
    printf '%s\n' "$program"
  done <"$MANIFEST"
}

program_count=0
fail_count=0
while IFS= read -r program; do
  program_count=$((program_count + 1))
  if ! run_one "$program"; then
    fail_count=$((fail_count + 1))
  fi
done < <(load_programs "$@")

if [ "$program_count" -eq 0 ]; then
  echo "[vm cpp exit diff] FAIL: no programs in manifest" >&2
  exit 1
fi
if [ "$fail_count" -ne 0 ]; then
  echo "[vm cpp exit diff] FAIL: $fail_count of $program_count" >&2
  exit 1
fi
echo "[vm cpp exit diff] ok ($program_count programs)" >&2
