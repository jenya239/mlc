#!/usr/bin/env bash
# TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU — scenario idle_frame_snapshot_stable (L2).
# Green after STEP=2; STEP=1 expects EXIT≠0.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/idle_frame_snapshot_stable.mlc"
OUT_DIR="${UX_IDLE_FRAME_SNAPSHOT_STABLE_OUT:-$ROOT_DIR/tmp/ux_idle_frame_snapshot_stable}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux idle_frame_snapshot_stable] FAIL: mlcc not found at $MLCC" >&2
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
if [ "$status" -ne 0 ]; then
  echo "[ux idle_frame_snapshot_stable] FAIL binary exit=$status (expected red until STEP=2)" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok idle_frame_snapshot_stable'; then
  echo "[ux idle_frame_snapshot_stable] FAIL: missing ux_ok idle_frame_snapshot_stable" >&2
  exit 1
fi
echo "[ux idle_frame_snapshot_stable] ok" >&2
