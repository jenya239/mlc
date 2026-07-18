#!/usr/bin/env bash
# TRACK_EDITOR_SESSION_CARET_RESTORE STEP=1 — scenario session_restore_caret (L1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/session_restore_caret.mlc"
OUT_DIR="${UX_SESSION_RESTORE_CARET_OUT:-$ROOT_DIR/tmp/ux_session_restore_caret}"
BIN_OUT="$OUT_DIR/bin"

if [ ! -x "$MLCC" ]; then
  echo "[ux session_restore_caret] FAIL: mlcc not found at $MLCC" >&2
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
  echo "[ux session_restore_caret] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok session_restore_caret'; then
  echo "[ux session_restore_caret] FAIL: missing ux_ok session_restore_caret" >&2
  exit 1
fi
echo "[ux session_restore_caret] ok" >&2
