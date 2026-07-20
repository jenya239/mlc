#!/usr/bin/env bash
# TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE STEP=1 — confirm L2 stub is still red.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/wrap_count_cache_stable.mlc"
OUT_DIR="$ROOT_DIR/tmp/ux_wrap_count_cache_red"
BIN_OUT="$OUT_DIR/bin"
FAIL_TOKEN="ux_fail wrap_count_cache_stable stub"

if [ ! -x "$MLCC" ]; then
  echo "[ux wrap_count_cache_red] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -eq 0 ]; then
  echo "[ux wrap_count_cache_red] FAIL: exit 0 (expected red)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q "$FAIL_TOKEN"; then
  echo "[ux wrap_count_cache_red] FAIL: missing $FAIL_TOKEN" >&2
  exit 1
fi

echo "ux_ok wrap_count_cache_red"
echo "[ux wrap_count_cache_red] ok" >&2
