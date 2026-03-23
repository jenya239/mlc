#!/usr/bin/env bash
# E2E tests for the self-hosted mlcc compiler.
# Usage: run_e2e.sh [mlcc_binary]
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
MLCC="${1:-$ROOT_DIR/compiler/out/mlcc}"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC="$ROOT_DIR/runtime/src/io/io.cpp $ROOT_DIR/runtime/src/core/string.cpp"

PASS=0; FAIL=0

run_test() {
  local name="$1"
  local src="$2"
  local expected="$3"
  local tmpdir; tmpdir=$(mktemp -d)

  if ! "$MLCC" -o "$tmpdir" "$src" 2>/tmp/e2e_err; then
    echo "FAIL $name: mlcc error: $(cat /tmp/e2e_err)"
    FAIL=$((FAIL+1)); rm -rf "$tmpdir"; return
  fi
  if ! g++ -std=c++20 -I "$tmpdir" -I "$RT_INC" "$tmpdir"/*.cpp $RT_SRC -o "$tmpdir/prog" 2>/tmp/e2e_err; then
    echo "FAIL $name: g++ error: $(cat /tmp/e2e_err | head -3)"
    FAIL=$((FAIL+1)); rm -rf "$tmpdir"; return
  fi
  local actual; actual=$("$tmpdir/prog" 2>&1)
  if [ "$actual" = "$expected" ]; then
    echo "PASS $name"
    PASS=$((PASS+1))
  else
    echo "FAIL $name: expected $(printf '%q' "$expected"), got $(printf '%q' "$actual")"
    FAIL=$((FAIL+1))
  fi
  rm -rf "$tmpdir"
}

run_test "hello"     "$SCRIPT_DIR/hello.mlc"     "hello world"
run_test "fibonacci" "$SCRIPT_DIR/fibonacci.mlc" "55"
run_test "result"    "$SCRIPT_DIR/result.mlc"    "6
division by zero"
run_test "sum_types" "$SCRIPT_DIR/sum_types.mlc" "circle: 75
rect: 12"

echo ""
echo "$PASS passed, $FAIL failed"
[ $FAIL -eq 0 ]
