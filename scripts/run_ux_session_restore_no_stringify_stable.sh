#!/usr/bin/env bash
# TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY — L2: restore tabs without stringify.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/session_restore_no_stringify.mlc"
OUT_DIR="${UX_SESSION_RESTORE_NO_STRINGIFY_OUT:-$ROOT_DIR/tmp/ux_session_restore_no_stringify}"
BIN_OUT="$OUT_DIR/bin"
SESSION="$ROOT_DIR/misc/editor/workspace/session.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux session_restore_no_stringify_stable] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if grep -q 'document_to_string' "$SESSION"; then
  echo "[ux session_restore_no_stringify_stable] FAIL: session still calls document_to_string" >&2
  exit 1
fi
if ! grep -q 'tab_set_open_buffer' "$SESSION"; then
  echo "[ux session_restore_no_stringify_stable] FAIL: session missing tab_set_open_buffer" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
mkdir -p "$ROOT_DIR/.tmp/editor_fixture"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux session_restore_no_stringify_stable] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok session_restore_no_stringify'; then
  echo "[ux session_restore_no_stringify_stable] FAIL: missing ux_ok session_restore_no_stringify" >&2
  exit 1
fi
echo "[ux session_restore_no_stringify_stable] ok" >&2
