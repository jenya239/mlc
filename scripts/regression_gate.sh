#!/usr/bin/env bash
# Regression gate: frozen suite stdout — Ruby bootstrap vs mlcc must match expected.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROGRAMS="$ROOT/specs/regression/programs"
EXPECTED="$ROOT/specs/regression/expected"
MANIFEST="$ROOT/specs/regression/manifest.tsv"
MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"
RT_INC="$ROOT/runtime/include"
RT_SRC="$ROOT/runtime/src/io/io.cpp $ROOT/runtime/src/core/string.cpp"
RUN_RUBY=(bundle exec ruby "$ROOT/bin/mlc")

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
  if ! g++ -std=c++20 -I "$tmpdir/out" -I "$RT_INC" "$tmpdir/out"/*.cpp $RT_SRC -o "$tmpdir/prog" 2>"$tmpdir/link.err"; then
    echo "g++ link failed:" >&2
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

PASS=0
FAIL=0

while IFS=$'\t' read -r name _expected_path; do
  [[ -z "$name" || "$name" == \#* ]] && continue
  source_file="$PROGRAMS/${name}.mlc"
  expected_file="$EXPECTED/${name}.txt"
  if [[ ! -f "$source_file" || ! -f "$expected_file" ]]; then
    echo "FAIL $name: missing source or expected file" >&2
    FAIL=$((FAIL + 1))
    continue
  fi

  tmpdir="$(mktemp -d)"
  ruby_out="$tmpdir/ruby.out"
  mlcc_out="$tmpdir/mlcc.out"

  if ! run_ruby_stdout "$source_file" "$tmpdir" >"$ruby_out"; then
    echo "FAIL $name: Ruby mlc failed" >&2
    cat "$tmpdir/ruby.err" >&2
    FAIL=$((FAIL + 1))
    rm -rf "$tmpdir"
    continue
  fi

  if ! run_mlcc_stdout "$source_file" "$tmpdir" >"$mlcc_out"; then
    FAIL=$((FAIL + 1))
    rm -rf "$tmpdir"
    continue
  fi

  program_ok=true
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
    PASS=$((PASS + 1))
  else
    FAIL=$((FAIL + 1))
  fi
  rm -rf "$tmpdir"
done < "$MANIFEST"

echo ""
echo "regression_gate: $PASS passed, $FAIL failed"
[[ $FAIL -eq 0 ]]
