#!/usr/bin/env bash
# Fuzz differential: mlcc --check-only vs Ruby checker (negative corpus + random seeds).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
CORPUS_DIR="$(cd "$(dirname "$0")/negative_corpus" && pwd)"
DIVERGENCES="$(cd "$(dirname "$0")" && pwd)/negative_corpus_known_divergences.txt"
CHECKER_NEGATIVE="$(cd "$(dirname "$0")" && pwd)/checker_negative_subset.txt"
RUBY_CHECKER="$ROOT_DIR/scripts/fuzz_ruby_checker_exit.rb"
RUBY_ERROR_COUNT="$ROOT_DIR/scripts/fuzz_ruby_checker_error_count.rb"
SEED_COUNT=8
TMPDIR="${TMPDIR:-/tmp}"
WORK=$(mktemp -d "$TMPDIR/mlcc_fuzz_diff_XXXXXX")
trap 'rm -rf "$WORK"' EXIT

declare -A KNOWN_MLCC_EXIT
declare -A KNOWN_RUBY_EXIT

fuzz_mix() {
  local seed=$1
  echo $(( (seed * 1664525 + 1013904223) & 0x7fffffff ))
}

generate_program() {
  local seed=$1
  local kind=$(($(fuzz_mix "$seed") % 12))
  case "$kind" in
    0) printf '%s\n' 'fn fuzz_entry() -> i32 = 0' ;;
    1) printf '%s\n' 'fn fuzz_entry() -> i32 = 1 + 2' ;;
    2) printf '%s\n' 'fn fuzz_entry() -> string = "s"' ;;
    3) printf '%s\n' 'fn fuzz_entry() -> bool = true' ;;
    4) printf '%s\n' "fn fuzz_entry() -> i32 = $(($(fuzz_mix $((seed + 1)) % 100)))" ;;
    5)
      local helper="helper_$(($(fuzz_mix $((seed + 2)) % 1000)))"
      printf '%s\n' "fn ${helper}() -> i32 = 42" "fn fuzz_entry() -> i32 = ${helper}()"
      ;;
    6) printf '%s\n' 'fn fuzz_entry() -> i32 = if true then 1 else 2' ;;
    7) printf '%s\n' 'fn fuzz_entry() -> i32 = do let x = 1; x end' ;;
    8) printf '%s\n' "fn fuzz_entry() -> i32 = match $(($(fuzz_mix $((seed + 3)) % 10))) { 1 => 0, _ => 1 }" ;;
    9)
      local type_name="FuzzStub_$(($(fuzz_mix $((seed + 4)) % 1000)))"
      printf '%s\n' "type ${type_name} = { tag: i32 }" "fn fuzz_entry() -> i32 = 0"
      ;;
    10)
      local type_name="FuzzRecord_$(($(fuzz_mix $((seed + 5)) % 1000)))"
      local field_value=$(($(fuzz_mix $((seed + 6)) % 100)))
      printf '%s\n' "type ${type_name} = { value: i32 }" "fn fuzz_entry() -> i32 = ${type_name} { value: ${field_value} }.value"
      ;;
    11)
      local inner_value=$(($(fuzz_mix $((seed + 7)) % 50)))
      printf '%s\n' "fn fuzz_entry() -> i32 = do" "  let outer = do" "    let inner = ${inner_value}" "    inner" "  end" "  outer" "end"
      ;;
  esac
}

load_known_divergences() {
  while IFS= read -r line || [ -n "$line" ]; do
    case "$line" in
      ''|'#'*) continue ;;
    esac
    read -r mlcc_exit ruby_exit filename _rest <<<"$line"
    KNOWN_MLCC_EXIT["$filename"]="$mlcc_exit"
    KNOWN_RUBY_EXIT["$filename"]="$ruby_exit"
  done <"$DIVERGENCES"
}

normalize_exit() {
  local code=$1
  if [ "$code" -gt 1 ]; then
    echo "crash"
  elif [ "$code" -eq 0 ]; then
    echo 0
  else
    echo 1
  fi
}

run_mlcc_check_only() {
  local path=$1
  set +e
  "$MLCC" --check-only "$path" >/dev/null 2>&1
  local code=$?
  set -e
  echo "$code"
}

run_ruby_checker() {
  local path=$1
  set +e
  bundle exec ruby -I"$ROOT_DIR/lib" "$RUBY_CHECKER" "$path" >/dev/null 2>&1
  local code=$?
  set -e
  echo "$code"
}

run_mlcc_error_count() {
  local path=$1
  set +e
  local output
  output=$("$MLCC" --check-only "$path" 2>&1)
  local code=$?
  set -e
  if [ "$code" -gt 1 ]; then
    echo "crash"
    return
  fi
  echo "$output" | grep -c '^error:' || true
}

run_ruby_error_count() {
  local path=$1
  set +e
  local output
  output=$(bundle exec ruby -I"$ROOT_DIR/lib" "$RUBY_ERROR_COUNT" "$path" 2>/dev/null)
  local code=$?
  set -e
  if [ "$code" -eq 2 ]; then
    echo "parse"
    return
  fi
  if [ "$code" -eq 3 ]; then
    echo "crash"
    return
  fi
  echo "$output"
}

assert_exit_parity() {
  local label=$1
  local path=$2
  local mlcc_code=$3
  local ruby_code=$4
  local mlcc_bin=$(normalize_exit "$mlcc_code")
  local ruby_bin=$(normalize_exit "$ruby_code")

  if [ "$mlcc_bin" = "crash" ]; then
    echo "[fuzz differential] FAIL $label: mlcc exit $mlcc_code (crash?)" >&2
    exit 1
  fi
  if [ "$ruby_bin" = "crash" ]; then
    echo "[fuzz differential] FAIL $label: Ruby checker exit $ruby_code (crash?)" >&2
    exit 1
  fi
  if [ "$mlcc_bin" != "$ruby_bin" ]; then
    echo "[fuzz differential] mismatch $label mlcc=$mlcc_code ruby=$ruby_code" >&2
    return 1
  fi
  return 0
}

load_known_divergences

corpus_count=0
corpus_mismatch_count=0

for path in "$CORPUS_DIR"/*.mlc; do
  [ -f "$path" ] || continue
  name=$(basename "$path")
  mlcc_code=$(run_mlcc_check_only "$path")
  ruby_code=$(run_ruby_checker "$path")
  mlcc_bin=$(normalize_exit "$mlcc_code")
  ruby_bin=$(normalize_exit "$ruby_code")

  if [ "$mlcc_bin" = "crash" ]; then
    echo "[fuzz differential] FAIL $name: mlcc exit $mlcc_code (crash?)" >&2
    exit 1
  fi
  if [ "$ruby_bin" = "crash" ]; then
    echo "[fuzz differential] FAIL $name: Ruby checker exit $ruby_code (crash?)" >&2
    exit 1
  fi

  if [ -n "${KNOWN_MLCC_EXIT[$name]+x}" ]; then
    if [ "$mlcc_bin" != "${KNOWN_MLCC_EXIT[$name]}" ] || [ "$ruby_bin" != "${KNOWN_RUBY_EXIT[$name]}" ]; then
      echo "[fuzz differential] FAIL $name: known divergence drift mlcc=$mlcc_code ruby=$ruby_code (expected mlcc=${KNOWN_MLCC_EXIT[$name]} ruby=${KNOWN_RUBY_EXIT[$name]})" >&2
      exit 1
    fi
  elif [ "$mlcc_bin" != "$ruby_bin" ]; then
    echo "[fuzz differential] mismatch $name mlcc=$mlcc_code ruby=$ruby_code" >&2
    corpus_mismatch_count=$((corpus_mismatch_count + 1))
  fi

  corpus_count=$((corpus_count + 1))
done

if [ "$corpus_count" -eq 0 ]; then
  echo "[fuzz differential] FAIL: no .mlc files in $CORPUS_DIR" >&2
  exit 1
fi

if [ "$corpus_mismatch_count" -ne 0 ]; then
  echo "[fuzz differential] FAIL: $corpus_mismatch_count unexpected corpus mismatch(es)" >&2
  exit 1
fi

known_count=${#KNOWN_MLCC_EXIT[@]}
parity_count=$((corpus_count - known_count))

seed=0
while [ "$seed" -lt "$SEED_COUNT" ]; do
  path="$WORK/fuzz_seed_${seed}.mlc"
  generate_program "$seed" >"$path"
  mlcc_code=$(run_mlcc_check_only "$path")
  ruby_code=$(run_ruby_checker "$path")
  if ! assert_exit_parity "seed=$seed" "$path" "$mlcc_code" "$ruby_code"; then
    exit 1
  fi
  seed=$((seed + 1))
done

checker_negative_count=0
while IFS= read -r filename || [ -n "$filename" ]; do
  case "$filename" in
    ''|'#'*) continue ;;
  esac
  path="$CORPUS_DIR/$filename"
  if [ ! -f "$path" ]; then
    echo "[fuzz differential] FAIL: missing checker-negative file $filename" >&2
    exit 1
  fi
  mlcc_errors=$(run_mlcc_error_count "$path")
  ruby_errors=$(run_ruby_error_count "$path")
  if [ "$mlcc_errors" = "crash" ] || [ "$ruby_errors" = "crash" ]; then
    echo "[fuzz differential] FAIL $filename: crash during error count" >&2
    exit 1
  fi
  if [ "$ruby_errors" = "parse" ]; then
    echo "[fuzz differential] FAIL $filename: Ruby parse error (expected checker errors)" >&2
    exit 1
  fi
  if [ "$mlcc_errors" -lt 1 ]; then
    echo "[fuzz differential] FAIL $filename: mlcc error count $mlcc_errors (expected >= 1)" >&2
    exit 1
  fi
  if [ "$ruby_errors" -lt 1 ]; then
    echo "[fuzz differential] FAIL $filename: Ruby error count $ruby_errors (expected >= 1)" >&2
    exit 1
  fi
  if [ "$mlcc_errors" != "$ruby_errors" ]; then
    echo "[fuzz differential] FAIL $filename: error count mlcc=$mlcc_errors ruby=$ruby_errors" >&2
    exit 1
  fi
  checker_negative_count=$((checker_negative_count + 1))
done <"$CHECKER_NEGATIVE"

if [ "$checker_negative_count" -eq 0 ]; then
  echo "[fuzz differential] FAIL: empty checker_negative_subset.txt" >&2
  exit 1
fi

echo "[fuzz differential] ok ($corpus_count corpus: $parity_count parity, $known_count known divergences; $SEED_COUNT seeds; $checker_negative_count checker-negative error-count parity)" >&2
