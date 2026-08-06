#!/usr/bin/env bash
# §104-6 slice 1: measure MIR lower coverage on an entry (default compiler/main.mlc).
# Exit 0 always — measurement only (fail-on-nonzero is a later slice).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${1:-$COMPILER_DIR/main.mlc}"

if [ ! -x "$MLCC" ]; then
  echo "mir-coverage: missing $MLCC — run compiler/build.sh first" >&2
  exit 1
fi

RAW="$("$MLCC" --check-only --mir-bootstrap-report "$ENTRY" 2>/dev/null || true)"
SUMMARY="$(printf '%s\n' "$RAW" | grep -E '^mir_functions=' | head -1 || true)"

if [ -z "$SUMMARY" ]; then
  echo "mir-coverage: no mir_functions= summary line from mlcc" >&2
  exit 1
fi

extract_field() {
  local key="$1"
  printf '%s\n' "$SUMMARY" | grep -oE "${key}=[0-9]+" | head -1 | cut -d= -f2
}

MIR_FUNCTIONS="$(extract_field mir_functions)"
LOWER_ERROR_COUNT="$(extract_field lower_error_count)"
CPP_SKIP="$(extract_field cpp_skip)"
CPP_OK="$(extract_field cpp_ok)"
SIMPLE="$(extract_field simple)"

if [ -z "${LOWER_ERROR_COUNT:-}" ]; then
  echo "mir-coverage: missing lower_error_count= in summary: $SUMMARY" >&2
  exit 1
fi

echo "mir-coverage: mir_functions=${MIR_FUNCTIONS} simple=${SIMPLE} cpp_ok=${CPP_OK} cpp_skip=${CPP_SKIP} lower_error_count=${LOWER_ERROR_COUNT}"
echo "$SUMMARY"
exit 0
