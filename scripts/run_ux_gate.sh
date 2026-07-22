#!/usr/bin/env bash
# TRACK_UX_HEADLESS — full UX regression gate.
#
# Auto-discovers every scripts/run_ux_*.sh (self-registering: a new track's
# STEP=1 script is picked up automatically, no manual list to keep in sync —
# see docs/GUI_UX_TESTING.md "Standing discipline"). Excludes:
#   - this script itself
#   - *_red.sh: STEP=1 red-state scaffolding proving a bug reproduces before
#     the fix: intentionally fails on a fixed tree, not a regression check.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

fail=0
count=0
run_one() {
  local name="$1"
  local script="$2"
  count=$((count + 1))
  if ! bash "$script"; then
    echo "[ux gate] FAIL $name" >&2
    fail=1
  else
    echo "[ux gate] ok $name"
  fi
}

for script in "$ROOT_DIR"/scripts/run_ux_*.sh; do
  base="$(basename "$script")"
  [ "$base" = "run_ux_gate.sh" ] && continue
  case "$base" in
    *_red.sh) continue ;;
  esac
  name="${base#run_ux_}"
  name="${name%.sh}"
  run_one "$name" "$script"
done

if [ "$fail" != 0 ]; then
  echo "[ux gate] FAILED ($count scenarios run)" >&2
  exit 1
fi
echo "[ux gate] all ok ($count scenarios)"
exit 0
