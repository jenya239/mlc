#!/usr/bin/env bash
# TRACK_LANG_RECORD_UPDATE STEP=2+ — anonymous update must typecheck.
# STEP=3 wires full compile/link/run into run_e2e.sh.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
MLCC="${1:-$ROOT_DIR/compiler/out/mlcc}"
SRC="$SCRIPT_DIR/record_update_anonymous.mlc"

if ! "$MLCC" --check-only "$SRC" 2>/tmp/record_update_anonymous_check.err; then
  echo "FAIL record_update_anonymous_check: $(cat /tmp/record_update_anonymous_check.err)" >&2
  exit 1
fi

echo "record_update_anonymous_check_ok"
exit 0
