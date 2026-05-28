#!/usr/bin/env bash
# Fuzz differential step 1: negative corpus exit-code parity mlcc --check-only vs Ruby checker.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
CORPUS_DIR="$(cd "$(dirname "$0")/negative_corpus" && pwd)"
DIVERGENCES="$(cd "$(dirname "$0")" && pwd)/negative_corpus_known_divergences.txt"
RUBY_CHECKER="$ROOT_DIR/scripts/fuzz_ruby_checker_exit.rb"

declare -A KNOWN_MLCC_EXIT
declare -A KNOWN_RUBY_EXIT

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

load_known_divergences

count=0
mismatch_count=0

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
    mismatch_count=$((mismatch_count + 1))
  fi

  count=$((count + 1))
done

if [ "$count" -eq 0 ]; then
  echo "[fuzz differential] FAIL: no .mlc files in $CORPUS_DIR" >&2
  exit 1
fi

if [ "$mismatch_count" -ne 0 ]; then
  echo "[fuzz differential] FAIL: $mismatch_count unexpected mismatch(es)" >&2
  exit 1
fi

known_count=${#KNOWN_MLCC_EXIT[@]}
parity_count=$((count - known_count))
echo "[fuzz differential] ok ($count files: $parity_count parity, $known_count known divergences)" >&2
