#!/usr/bin/env bash
# TRACK_UX_HEADLESS — run all registered UX gates (grows with steps).
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

fail=0
run_one() {
  local name="$1"
  local script="$2"
  if ! bash "$script"; then
    echo "[ux gate] FAIL $name" >&2
    fail=1
  else
    echo "[ux gate] ok $name"
  fi
}

run_one expect_unit "$ROOT_DIR/scripts/run_ux_expect_unit.sh"
run_one driver_unit "$ROOT_DIR/scripts/run_ux_driver_unit.sh"

if [ "$fail" != 0 ]; then
  echo "[ux gate] FAILED" >&2
  exit 1
fi
echo "[ux gate] all ok"
exit 0
