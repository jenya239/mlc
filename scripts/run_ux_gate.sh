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
run_one probe_unit "$ROOT_DIR/scripts/run_ux_probe_unit.sh"
run_one tab_strip_click "$ROOT_DIR/scripts/run_ux_tab_strip_click.sh"
run_one newline_caret "$ROOT_DIR/scripts/run_ux_newline_caret.sh"
run_one overflow_editor_text "$ROOT_DIR/scripts/run_ux_overflow_editor_text.sh"
run_one wheel_scroll "$ROOT_DIR/scripts/run_ux_wheel_scroll.sh"

if [ "$fail" != 0 ]; then
  echo "[ux gate] FAILED" >&2
  exit 1
fi
echo "[ux gate] all ok"
exit 0
