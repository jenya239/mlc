#!/usr/bin/env bash
# TRACK_LANG_RECORD_UPDATE STEP=1 — red gate: anonymous update must NOT compile yet.
# When STEP=3 turns green, replace this with a PASS path in run_e2e.sh and delete/retire this script.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
MLCC="${1:-$ROOT_DIR/compiler/out/mlcc}"
SRC="$SCRIPT_DIR/record_update_anonymous.mlc"
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

if "$MLCC" -o "$TMPDIR" "$SRC" 2>"$TMPDIR/err"; then
  echo "FAIL record_update_anonymous_red: mlcc unexpectedly succeeded" >&2
  exit 1
fi

echo "record_update_anonymous_red_ok"
exit 0
