#!/usr/bin/env bash
# MIR bootstrap report + timing (TRACK_MIR STEP=8).
# Usage: compiler/scripts/mir_bootstrap_report.sh [entry.mlc]
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${1:-$COMPILER_DIR/main.mlc}"

if [ ! -x "$MLCC" ]; then
  echo "mir_bootstrap_report: missing $MLCC — run compiler/build.sh first" >&2
  exit 1
fi

measure_seconds() {
  local output
  if ! output="$(/usr/bin/time -f '%e' "$@" 2>&1)"; then
    echo "$output" >&2
    return 1
  fi
  echo "$output" | tail -1
}

echo "mir_bootstrap_report: entry=$ENTRY mlcc=$MLCC"

check_only_sec="$(measure_seconds "$MLCC" --check-only "$ENTRY")"
echo "check_only_sec=${check_only_sec}"

report_sec="$(measure_seconds "$MLCC" --check-only --mir-bootstrap-report "$ENTRY")"
echo "mir_bootstrap_report_sec=${report_sec}"

echo "--- report output ---"
"$MLCC" --check-only --mir-bootstrap-report "$ENTRY" 2>/dev/null | sed -n '/^--- mir-bootstrap-report:/,/^$/p'
