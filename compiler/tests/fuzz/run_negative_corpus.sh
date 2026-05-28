#!/usr/bin/env bash
# Negative corpus: static invalid .mlc files — mlcc must not crash (exit 0 or 1 only).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
CORPUS_DIR="$(cd "$(dirname "$0")/negative_corpus" && pwd)"

run_mlcc_expect_no_crash() {
  local path=$1
  local label=$2
  set +e
  "$MLCC" --check-only "$path" >/dev/null 2>&1
  local code=$?
  set -e
  if [ "$code" -gt 1 ]; then
    echo "[negative corpus] FAIL $label: mlcc exit $code (crash?)" >&2
    exit 1
  fi
}

count=0
for path in "$CORPUS_DIR"/*.mlc; do
  [ -f "$path" ] || continue
  name=$(basename "$path")
  run_mlcc_expect_no_crash "$path" "$name"
  count=$((count + 1))
done

if [ "$count" -eq 0 ]; then
  echo "[negative corpus] FAIL: no .mlc files in $CORPUS_DIR" >&2
  exit 1
fi

echo "[negative corpus] ok ($count files)" >&2
