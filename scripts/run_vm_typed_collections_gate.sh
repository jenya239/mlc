#!/usr/bin/env bash
# Non-i32 array push/get via mlcc --run (TRACK_VM_TYPED_COLLECTIONS STEP=2).
set -euo pipefail
# Fail the script if any pipeline stage fails when used under `| tee`.


ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"

if [ ! -x "$MLCC" ]; then
  echo "[vm typed collections] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

run_expect() {
  local program="$1"
  local expected="$2"
  local name
  name="$(basename "$program" .mlc)"
  set +e
  "$MLCC" --run "$program" >/dev/null
  local status=$?
  set -e
  if [ "$status" -ne "$expected" ]; then
    echo "[vm typed collections] FAIL $name exit=$status expected=$expected" >&2
    exit 1
  fi
  echo "[vm typed collections] ok $name exit=$status" >&2
}

run_expect "$ROOT_DIR/misc/examples/vm_array_of_string.mlc" 4
run_expect "$ROOT_DIR/misc/examples/vm_array_of_record.mlc" 9
run_expect "$ROOT_DIR/misc/examples/vm_collections_corpus.mlc" 7
echo "[vm typed collections] ok" >&2

