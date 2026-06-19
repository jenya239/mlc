#!/usr/bin/env bash
# mlcc vs mlcc2 checker parity on minimal corpus (TRACK_MLCC2_CHECKER_PARITY).
# After STEP=3 fix: both --check-only exit 0 on corpus.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
MLCC2="${2:-$ROOT_DIR/.tmp_selfhost/mlcc2}"
CORPUS_DIR="$(cd "$(dirname "$0")/mlcc2_parity_corpus" && pwd)"

if [ ! -x "$MLCC2" ]; then
  echo "[mlcc2 checker parity] skip: mlcc2 not found at $MLCC2 (build via self-host gate)" >&2
  exit 0
fi

check_file() {
  local path=$1
  local name
  name=$(basename "$path")
  set +e
  "$MLCC" --check-only "$path" >/dev/null 2>&1
  local mlcc_exit=$?
  "$MLCC2" --check-only "$path" >/dev/null 2>&1
  local mlcc2_exit=$?
  set -e
  if [ "$mlcc_exit" -ne 0 ]; then
    echo "[mlcc2 checker parity] FAIL $name: mlcc exit $mlcc_exit (expected 0)" >&2
    exit 1
  fi
  if [ "$mlcc2_exit" -ne 0 ]; then
    echo "[mlcc2 checker parity] FAIL $name: mlcc2 exit $mlcc2_exit (expected 0)" >&2
    exit 1
  fi
  echo "[mlcc2 checker parity] ok $name (mlcc=0 mlcc2=0)" >&2
}

count=0
for path in "$CORPUS_DIR"/*.mlc; do
  [ -f "$path" ] || continue
  check_file "$path"
  count=$((count + 1))
done

if [ "$count" -eq 0 ]; then
  echo "[mlcc2 checker parity] FAIL: no .mlc in $CORPUS_DIR" >&2
  exit 1
fi

echo "[mlcc2 checker parity] ok ($count files)" >&2
