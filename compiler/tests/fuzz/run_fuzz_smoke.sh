#!/usr/bin/env bash
# Fuzz smoke: mlcc --check-only on generated programs must not crash (exit 0 or 1 only).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
TMPDIR="${TMPDIR:-/tmp}"
WORK=$(mktemp -d "$TMPDIR/mlcc_fuzz_XXXXXX")
trap 'rm -rf "$WORK"' EXIT

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

run_mlcc_expect_no_crash() {
  local path=$1
  local label=$2
  set +e
  timeout 10 "$MLCC" --check-only "$path" >/dev/null 2>&1
  local code=$?
  set -e
  if [ "$code" -eq 124 ]; then
    echo "[fuzz smoke] FAIL $label: mlcc timed out (hang?)" >&2
    exit 1
  fi
  if [ "$code" -gt 1 ]; then
    echo "[fuzz smoke] FAIL $label: mlcc exit $code (crash?)" >&2
    exit 1
  fi
}

random_byte_sizes="0 1 8 64 256 1024"

echo "[fuzz smoke] mlcc=$MLCC (32 generated + 2 garbage + random-byte)" >&2

seed=0
while [ "$seed" -lt 32 ]; do
  path="$WORK/fuzz_${seed}.mlc"
  generate_program "$seed" >"$path"
  run_mlcc_expect_no_crash "$path" "seed=$seed"
  seed=$((seed + 1))
done

printf '%s\n' 'fn @@@' >"$WORK/garbage_parse.mlc"
run_mlcc_expect_no_crash "$WORK/garbage_parse.mlc" 'garbage_parse'

printf '%s\n' 'fn f() -> i32 = "x"' >"$WORK/garbage_types.mlc"
run_mlcc_expect_no_crash "$WORK/garbage_types.mlc" 'garbage_types'

byte_index=0
for byte_size in $random_byte_sizes; do
  path="$WORK/random_bytes_${byte_size}.mlc"
  if [ "$byte_size" -eq 0 ]; then
    : >"$path"
  else
    head -c "$byte_size" /dev/urandom >"$path"
  fi
  run_mlcc_expect_no_crash "$path" "random_bytes_${byte_size}"
  byte_index=$((byte_index + 1))
done

for random_seed in $(seq 0 7); do
  byte_size=$(($(fuzz_mix "$random_seed") % 512 + 1))
  path="$WORK/random_seed_${random_seed}.mlc"
  head -c "$byte_size" /dev/urandom >"$path"
  run_mlcc_expect_no_crash "$path" "random_seed_${random_seed}_size_${byte_size}"
done

echo "[fuzz smoke] ok" >&2
