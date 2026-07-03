#!/usr/bin/env bash
# mlcc vs mlcc_bootstrap codegen parity + bootstrap self-host diff (TRACK_SELF_HOST_BOOTSTRAP STEP=4).
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
MLCC_BOOTSTRAP="${2:-$COMPILER_DIR/out/bootstrap/mlcc_bootstrap}"
ENTRY="$COMPILER_DIR/main.mlc"
WORK_DIR="${3:-$ROOT_DIR/.tmp_selfhost}"

if [ ! -x "$MLCC" ]; then
  echo "[mlcc bootstrap parity] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if [ ! -x "$MLCC_BOOTSTRAP" ]; then
  echo "[mlcc bootstrap parity] skip: mlcc_bootstrap not found at $MLCC_BOOTSTRAP (run MLCC_BOOTSTRAP=1 compiler/build.sh)" >&2
  exit 0
fi

mkdir -p "$WORK_DIR"
MLCC_OUT="$WORK_DIR/mlcc_codegen"
BS_OUT="$WORK_DIR/bs_codegen"
P1="$WORK_DIR/p1"
P2="$WORK_DIR/p2"
BS2="$WORK_DIR/bootstrap2"

rm -rf "$MLCC_OUT" "$BS_OUT" "$P1" "$P2"
mkdir -p "$MLCC_OUT" "$BS_OUT" "$P1" "$P2"

echo "[mlcc bootstrap parity] mlcc codegen …" >&2
"$MLCC" -o "$MLCC_OUT" "$ENTRY"

echo "[mlcc bootstrap parity] mlcc_bootstrap codegen …" >&2
"$MLCC_BOOTSTRAP" -o "$BS_OUT" "$ENTRY"

echo "[mlcc bootstrap parity] diff mlcc vs mlcc_bootstrap …" >&2
if ! diff -rq "$MLCC_OUT" "$BS_OUT"; then
  echo "[mlcc bootstrap parity] FAIL: mlcc and mlcc_bootstrap codegen differ" >&2
  exit 1
fi
echo "[mlcc bootstrap parity] ok codegen parity" >&2

echo "[mlcc bootstrap parity] bootstrap self-host chain …" >&2
"$MLCC_BOOTSTRAP" -o "$P1" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$P1" "$BS2"
"$BS2" -o "$P2" "$ENTRY"

if ! diff -rq --exclude=obj "$P1" "$P2"; then
  echo "[mlcc bootstrap parity] FAIL: bootstrap self-host diff" >&2
  exit 1
fi

echo "[mlcc bootstrap parity] ok (codegen + self-host)" >&2
