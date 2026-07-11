#!/usr/bin/env bash
# Regression gate: frozen suite stdout — Ruby bootstrap vs mlcc must match expected.
# Programs are independent; run them in parallel (job pool, like build_bin.sh).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROGRAMS="$ROOT/specs/regression/programs"
EXPECTED="$ROOT/specs/regression/expected"
MANIFEST="$ROOT/specs/regression/manifest.tsv"
MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"
RT_INC="$ROOT/runtime/include"
RT_SRC="$ROOT/runtime/src/io/io.cpp $ROOT/runtime/src/core/string.cpp"
RUN_RUBY=(bundle exec ruby "$ROOT/bin/mlc")
CXX="${MLC_CXX:-g++}"
JOBS="${MLC_JOBS:-$(nproc 2>/dev/null || echo 4)}"

if [[ ! -x "$MLCC" ]]; then
  echo "regression_gate: mlcc not found: $MLCC" >&2
  exit 1
fi

run_ruby_stdout() {
  local source_file="$1"
  local tmpdir="$2"
  "${RUN_RUBY[@]}" -o "$tmpdir" "$source_file" 2>"$tmpdir/ruby.err"
}

run_mlcc_stdout() {
  local source_file="$1"
  local tmpdir="$2"
  if ! "$MLCC" -o "$tmpdir/out" "$source_file" 2>"$tmpdir/mlcc.err"; then
    echo "mlcc compile failed:" >&2
    cat "$tmpdir/mlcc.err" >&2
    return 1
  fi
  if ! $CXX -std=c++20 -I "$tmpdir/out" -I "$RT_INC" "$tmpdir/out"/*.cpp $RT_SRC -o "$tmpdir/prog" 2>"$tmpdir/link.err"; then
    echo "$CXX link failed:" >&2
    head -5 "$tmpdir/link.err" >&2
    return 1
  fi
  "$tmpdir/prog" 2>"$tmpdir/run.err"
}

compare_output() {
  local name="$1"
  local label="$2"
  local actual_file="$3"
  local expected_file="$4"
  if diff -u "$expected_file" "$actual_file" >"$actual_file.diff"; then
    echo "PASS $name ($label vs expected)"
    return 0
  fi
  echo "FAIL $name ($label vs expected)" >&2
  cat "$actual_file.diff" >&2
  return 1
}

RESULTS_DIR="$(mktemp -d)"
trap 'rm -rf "$RESULTS_DIR"' EXIT

run_one() {
  local name="$1"
  local log="$RESULTS_DIR/$name.log"
  {
    local source_file="$PROGRAMS/${name}.mlc"
    local expected_file="$EXPECTED/${name}.txt"
    if [[ ! -f "$source_file" || ! -f "$expected_file" ]]; then
      echo "FAIL $name: missing source or expected file" >&2
      echo FAIL >"$RESULTS_DIR/$name.status"
      return
    fi

    local tmpdir ruby_out mlcc_out
    tmpdir="$(mktemp -d)"
    ruby_out="$tmpdir/ruby.out"
    mlcc_out="$tmpdir/mlcc.out"

    if ! run_ruby_stdout "$source_file" "$tmpdir" >"$ruby_out"; then
      echo "FAIL $name: Ruby mlc failed" >&2
      cat "$tmpdir/ruby.err" >&2
      echo FAIL >"$RESULTS_DIR/$name.status"
      rm -rf "$tmpdir"
      return
    fi

    if ! run_mlcc_stdout "$source_file" "$tmpdir" >"$mlcc_out"; then
      echo FAIL >"$RESULTS_DIR/$name.status"
      rm -rf "$tmpdir"
      return
    fi

    local program_ok=true
    compare_output "$name" "ruby" "$ruby_out" "$expected_file" || program_ok=false
    compare_output "$name" "mlcc" "$mlcc_out" "$expected_file" || program_ok=false

    if diff -u "$ruby_out" "$mlcc_out" >"$tmpdir/ruby_mlcc.diff"; then
      echo "PASS $name (ruby vs mlcc)"
    else
      echo "FAIL $name (ruby vs mlcc)" >&2
      cat "$tmpdir/ruby_mlcc.diff" >&2
      program_ok=false
    fi

    if $program_ok; then
      echo PASS >"$RESULTS_DIR/$name.status"
    else
      echo FAIL >"$RESULTS_DIR/$name.status"
    fi
    rm -rf "$tmpdir"
  } >"$log" 2>&1
}

NAMES=()
while IFS=$'\t' read -r name _expected_path; do
  [[ -z "$name" || "$name" == \#* ]] && continue
  NAMES+=("$name")
done < "$MANIFEST"

PIDS=()
for name in "${NAMES[@]}"; do
  run_one "$name" &
  PIDS+=($!)
  if [[ ${#PIDS[@]} -ge $JOBS ]]; then
    wait "${PIDS[@]}"
    PIDS=()
  fi
done
[[ ${#PIDS[@]} -eq 0 ]] || wait "${PIDS[@]}"

PASS=0
FAIL=0
for name in "${NAMES[@]}"; do
  cat "$RESULTS_DIR/$name.log"
  if [[ "$(cat "$RESULTS_DIR/$name.status" 2>/dev/null)" == "PASS" ]]; then
    PASS=$((PASS + 1))
  else
    FAIL=$((FAIL + 1))
  fi
done

echo ""
echo "regression_gate: $PASS passed, $FAIL failed"
if [[ $FAIL -ne 0 ]]; then
  exit 1
fi

echo ""
echo "regression_gate: examples compile+link sweep"
bash "$ROOT/scripts/run_examples_compile_sweep.sh"
